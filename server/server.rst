Some server configurations
=========================================

iptables
--------

* use package iptables-persistent (Debian)
* for packet forwarding, edit `/etc/sysctl.conf`

SSL keys and certs for different services
-----------------------------------------

* lighttpd: cert and key in ``ssl.pemfile``, and chain in ``ssl.ca-file``
* matrix-synapse: cert and key in ``tls_private_key_path``, and chain in ``tls_certificate_path``

Configure ejabberd
------------------

Modify the ejabberd.yml given by the distro according to the document in ejabberd.im.

Before ejabberd 17.11, we need to put cert, intermediate cert and key in `certfile`, and starting from ejabberd 17.11, we can use the `certfiles` option::

  certfiles:
    - "/etc/ssl/xmpp/live/cnjabber.net/*.pem"

Run ``ejabberdctl foreground`` to see if there is any error or warning. In Arch Linux, HOME needs to be set to /var/lib/ejabberd for user jabber::

  sudo -u jabber /usr/bin/env HOME=/var/lib/ejabberd ejabberdctl <sub cmd>
