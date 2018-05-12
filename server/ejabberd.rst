XMPP 服务器配置指南
===========================

XMPP 是一个具有高度扩展性的通讯协议。在自由软件和黑客社区中，XMPP 被广泛使用，它具有以下特点：

- 去中心化的联邦式协议
- 有成熟的客户端和服务端软件

  + 桌面客户端：Gajim, Psi/Psi+, Pidgin/Adium, Dino, Bitlbee, weechat
  + Android客户端：Conversations, Xabber
  + iOS客户端：ChatSecure/Zom
  + 服务端：ejabberd, prosody, openfire
- 大多数客户端支持端到端加密：OpenPGP, OTR, OMEMO
- 服务器开销低

以下我将介绍如何用 ejabberd 搭建一个支持多种常用 XMPP 扩展(XEP) 的 XMPP 服务器。

ejabberd 简介
-------------

ejabberd 是一个用 Erlang 编写的 XMPP 服务器。Erlang 是一个函数式编程语言，它的并发模型非常适用于编写高并发程序。ejabberd 可以在一个节点上处理百万量级的并发请求。Conversations 官方组建的服务器 conversations.im 即使用 ejabberd，并且 Conversations 社区的人也有参与 ejabberd 的开发。

ejabberd 每几个月会发布一个更新。我的服务器使用滚动更新的发行版 Arch Linux，它的软件仓库中的 ejabberd 在新版本的 tarball 出来之后就会更新，从而我可以一直使用最新版的 ejabberd. ejabberd 在新的版本中会改变一些配置文件的配置项，因此建议管理员在使用最新的 ejabberd 时参考官方的文档。以下我的配置文件基于最新的 ejabberd 18.04 版本。

ejabberd 配置
--------------

ejabberd 的配置文件为 YAML 格式。Arch 的 ejabberd 的配置文件为 /etc/ejabberd/ejabberd.yml.

首先是配置域名。一个 XMPP 服务器可以提供多个域名的 XMPP 服务，例如我的 XMPP 服务器有 cnjabber.net 和 wehack.space 两个域名，就在配置文件中写::

  hosts:
    - "cnjabber.net"
    - "wehack.space"

之后很重要的一点是 TLS 的配置。目前 XMPP 使用 STARTTLS 方案，和明文传输同使用 5222 端口，建议强制使用 STARTTLS. 在新版的 ejabberd 中，TLS 密钥和证书的路径使用 ``certfiles`` 配置项指定。

::

  certfiles:
    - "/etc/letsencrypt/live/cnjabber.net/*.pem"
    - "/etc/letsencrypt/live/wehack.space/*.pem"
  
  define_macro:
    'CIPHERS': "ECDHE+CHACHA20:ECDHE+AESGCM:DHE+AESGCM:DHE+CHACHA20:!3DES:!aNULL:!eNULL:!MEDIUM@STRENGTH"
    'TLSOPTS':
      - "no_sslv2"
      - "no_sslv3"
      - "no_tlsv1"
      - "no_tlsv1_1"
      - "cipher_server_preference"
      - "no_compression"
    'DHFILE': "/etc/ejabberd/dhparam.pem"
  
  listen:
    -
      port: 5222
      ip: "::"
      module: ejabberd_c2s
      starttls: true
      protocol_options: 'TLSOPTS'
      dhfile: 'DHFILE'
      ciphers: 'CIPHERS'
      starttls_required: true
      max_stanza_size: 65536
      shaper: c2s_shaper
      access: c2s
    -
      port: 5269
      ip: "::"
      module: ejabberd_s2s_in
  
  ###.  ==================
  ###'  S2S GLOBAL OPTIONS
  
  s2s_use_starttls: required
  s2s_protocol_options: 'TLSOPTS'
  s2s_dhfile: 'DHFILE'
  s2s_ciphers: "ECDHE+HIGH:DHE+HIGH:!SHA1"
  
在这里，我们用 TLSOPTS 宏指定禁用 TLS 1.2 之前的 TLS 协议，CIPHERS 宏指定 TLS cipher suite 使用带有前向安全和 AEAD 的算法，为了服务器间的兼容性，在 s2s_ciphers 中我放松了一点 cipher suite 的要求。注意 listen 中的 starttls_required 和 s2s_use_starttls, 我都设置为必须启用加密连接。

certfiles 中是我的 Let's Encrypt 证书和密钥的路径，所有 ejabberd 要用的都要列出来。DHFILE 是 DHE 密钥交换需要的的密钥交换参数，可以用 ``openssl dhparam 3072`` 生成一份。

之后是 ejabberd 模块的配置。ejabberd 支持的很多特性，包括 XMPP 扩展，都通过它的模块实现。这里我介绍几个比较常用的模块：

- mod_pubsub: pubsub 扩展协议的支持，OMEMO 端到端加密需要用基于 pubsub 的 PEP 扩展发布用户公钥等信息，因此还建议在它下面的 plugins 配置中要加上 pep，注意 ejabberd 默认文件的注释中注明 pep 需要 mod_caps，因此还要启用 mod_caps.
- mod_stream_mgmt: 流管理，建议启用。
- mod_mam: 消息存档管理，对于存储离线消息有用，建议启用。
- mod_http_upload: HTTP 文件上传扩展，有了它，用户可以向 XMPP 服务器上传文件供其他人下载。
- mod_muc: 多人聊天，如果你需要允许用户建立在服务器上的聊天室，需要启用。不启用此模块并不影响使用其他服务器的聊天室。
- mod_version: 启用它可以使其他人查询服务器使用的软件及其版本，我选择禁用它以避免攻击者探测服务器软件及版本。
- mod_register: 启用它可以使用户在服务器上注册帐号，此外，已有用户修改密码也需要此模块，因此建议启用。如果不想开放注册，可以修改相关权限选项禁用注册。

因此，最终我的配置文件中的相关选项如下::

  ##
  ## Modules enabled in all ejabberd virtual hosts.
  ##
  modules:
    mod_caps: {}
    mod_http_upload:
      host: "file.@HOST@"
      docroot: "/srv/http/sites/jabber-file/@HOST@"
      put_url: "https://file.@HOST@/put"
      get_url: "https://file.@HOST@"
    mod_http_upload_quota:
      max_days: 7
    mod_mam: {} # for xep0313, mnesia is limited to 2GB, better use an SQL backend
    mod_pubsub:
      access_createnode: pubsub_createnode
      ignore_pep_from_offline: true
      last_item_cache: false
      plugins:
        - "flat"
        - "hometree"
        - "pep" # pep requires mod_caps
    mod_register:
      captcha_protected: true
      password_strength: 32
      access_from: deny
      access: register
    ## mod_version: {}
    mod_stream_mgmt: {}

HTTP上传及 web 服务器的配置
-----------------------------

在以上配置中，我启用了 HTTP 上传，因此在 ejabberd 的 listen 部分要加上 HTTP 上传相关的配置::

  listen:
    -
    port: 5222
    ip: "::"
    module: ejabberd_c2s
    # ...
    # ...
    -
      port: 5444
      ip: "127.0.0.1"
      module: ejabberd_http
      request_handlers:
        "": mod_http_upload
      tls: false
 
在这里，我把 HTTP 上传服务设置为 127.0.0.1 的 5444 端口，并且不开 TLS，我要用我的 HTTP 服务器把对外的 HTTP 请求转发到这个端口。

同时，在 mod_http_upload 中，我设置了 docroot，我要在 HTTP 服务器中设置好文件路径，使得用户可以下载上传的文件。

我用的 HTTP 服务器是 Caddy, 对域名 wehack.space 的相关配置如下::

  https://file.wehack.space {
          tls /etc/letsencrypt/live/wehack.space/fullchain.pem /etc/letsencrypt/live/wehack.space/privkey.pem {
                  protocols tls1.2 tls1.2
                  must_staple
          }
          header / {
                  Strict-Transport-Security "max-age=31536000;"
                  X-XSS-Protection "1; mode=block"
                  X-Content-Type-Options "nosniff"
                  X-Frame-Options "SAMEORIGIN"
          }
  
          proxy /put 127.0.0.1:5444 {
                  transparent
          }
  
          root /srv/http/sites/jabber-file/wehack.space
  }
 
对其他域名也需要进行这样的配置。于是对 HTTP 上传的支持就配置好了。

总结
----

以上是我配置 ejabberd XMPP 服务器的一些笔记。最后，我们启动服务后，用 Conversations 或 Gajim 连接配置好的 XMPP 服务器后，可以查看服务器信息，看看想要的 XEP 是否已经启用。

如果有什么疑问，欢迎和我交流。
