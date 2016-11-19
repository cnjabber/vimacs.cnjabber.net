Some server configuration on cnjabber.net
=========================================

iptables
--------

* use package iptables-persistent
* for packet forwarding, edit `/etc/sysctl.conf`

SSL keys and certs for different services
-----------------------------------------

* lighttpd: cert and key in ``ssl.pemfile``, and chain in ``ssl.ca-file``
* matrix-synapse: cert and key in ``tls_private_key_path``, and chain in ``tls_certificate_path``
* ejabberd: cert, intermediate cert and key in ``certfile``
