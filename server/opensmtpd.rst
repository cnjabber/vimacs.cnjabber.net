opensmtpd 和邮件服务配置
=========================

opensmtpd 是一个轻量的 SMTP 实现，同时提供 sendmail 程序。

我需要让我的 BBS 通过 sendmail 发邮件，而不需要收邮件，因此配置比较简单。

在 DNS 设置 TXT SFP 记录 ``v=spf1 ip4:xxx.xxx.xxx.xxx ip6:xxxx::xxxx -all``

最后，有的服务器要求发送方给出一个合法的域名作为主机名 (domain.ltd 形式)，因此需要在 ``/etc/smtpd/mailname`` 加入域名::

  echo wehack.space > /etc/smtpd/mailname

最后，安装 opensmtpd 并启用服务。
