关于 OpenPGP
============

OpenPGP 是一个加密标准，利用公钥密码和对称加密技术保护用户的的隐私数据。不少网友（包括我）在自己的网站上公布了自己的 OpenPGP 密钥指纹，因此我下面介绍如何从一个人的 OpenPGP 密钥指纹获取他的邮箱地址。

**注意：建议和这个人见面以确认该密钥的确属于这个人！**


从密钥服务器的网页端获取
------------------------

网上有不少知名的密钥服务器，可以从这些服务器查询一个 OpenPGP 密钥::

- https://pgp.mit.edu/
- http://zimmerman.mayfirst.org/
- http://pool.sks-keyservers.net/

提醒一下，如果你不会用 OpenPGP 相关工具，建议用 https://pgp.mit.edu，因为它支持 HTTPS.

连上密钥服务器的网站后，在第一个搜索输入框中填上密钥指纹，把空格全部去掉，并且在前面加上 ``0x``,例如要查询 ``13EB BDBE DE7A 1277 5DFD B1BA BB57 2E0E 2D18 2910``,就搜索::

  0x13EBBDBEDE7A12775DFDB1BABB572E0E2D182910

然后能看到搜索结果::

  Type bits/keyID     Date       User ID

  pub  4096R/2D182910 2009-08-25 Gentoo Linux Release Engineering (Automated Weekly Release Key) <releng@gentoo.org>

在 User ID 那一栏就能看到邮箱 releng@gentoo.org 了。


利用 GnuPG 获取
---------------

用 GnuPG 也很简单，还是以上面的那个密钥为例，直接执行 ``gpg --recv-keys 0x13EBBDBEDE7A12775DFDB1BABB572E0E2D182910`` 即可。
