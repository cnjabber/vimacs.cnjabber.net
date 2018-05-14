Some VPS
========

除性价比之外，管理的方便程度也是选择VPS时要考虑的因素。

* SolusVM管理起来不方便，尤其是很多浏览器已经淘汰了Java applet的情况下，因此不考虑 AlphaRacks, HostMyBytes
* 支持LiveCD启动的VPS在安装系统上比较方便，可以使用 `arch-bootstrap <https://github.com/tokland/arch-bootstrap>`__


如何在KVM VPS上安装Arch
------------------------

虽然有 vps2arch 等自动化的工具，不过学会手动安装的方法还是有点用处的。

* 注意记下网络配置 (Debian系一般是/etc/network/interfaces)，不少 VPS 使用静态地址配置，尤其是 IPv6 的配置。建议收集一下内核命令行、模块等内核信息。
* 下载 archlinux-bootstrap 的 tarball, 解包至 /, 从而 rootfs 的路径为 /root.x86_64
* 安装并运行 haveged, 从而缩短 pacman-key --init 的时间
* 编辑 /root.x86_64/etc/pacman.d/mirrorlist, /root.x86_64/etc/resolv.conf
* umount 一些不必要的挂载点
* chroot 至 /root.x86_64, pacman-key --init, pacman-key --populate archlinux
* 挂载 /dev/vda1 等分区，删掉里面的 /bin, /sbin, /usr, /etc, ...
* 使用 pacstrap 安装系统，此后和 Arch 的常规安装流程相同


KVM
---

KVM是目前开源界最成熟，兼容性最好的虚拟化方案。

* `TinyKVM <https://tinykvm.com/>`__: 规格较低的VPS，管理方便
* `VMHaus <https://www.vmhaus.com/>`__: 性价比不错，从和TinyKVM规格相当的$15/y的低配到高配的选项都有，美国机房对国内线路有优化，支持Paypal和支付宝，用WebVNC管理
* `DigitalOcean <https://digitalocean.com/>`__: 价钱有点高，支持PayPal
* `Linode <https://www.linode.com/>`__: 要求有信用卡
* `Lunanode <https://www.lunanode.com/>`__: 内存大，IPv6不是很好，支持LiveCD启动；网络有Cogent和OVH两种；支持PayPal
* `Contabo <https://contabo.com/>`__ 和 `OVH <https://www.ovh.com/us/>`__: #matrix-dev:matrix.org 那边有两名网友用的VPS，性价比非常高
* `Scaleway <https://www.scaleway.com/>`__: 也是一个很有性价比的提供商，需要用信用卡。在FOSDEM 2018上提到他们使用coreboot.
* `gigsgigscloud <https://www.gigsgigscloud.com/>`__: 一家有名气的提供中国线路的VPS提供商

OpenVZ
------

OpenVZ VPS主要有以下问题：

* 无法控制内核：内核版本过老时，无法使用新的libc (kernel too old)，无法使用tun,tap,wireguard等内核模块
* 大多数超售严重

因此大多数用OpenVZ VPS的用户仅用于跑shadowsocks, haproxy等网络转发应用

* Ramnode: OpenVZ 和 KVM 都有，性价比一般
* `LowEndSpirit <https://clients.inceptionhosting.com/cart.php?gid=13>`__: IPv6，BBS上有人 `评价不错 <https://www.bdwm.net/v2/post-read.php?bid=484&threadid=15918334>`__

Xen
---

注意Xen对新的内核支持可能不太好。

* Gandi
* `prgmr <https://prgmr.com/xen/>`__: *The Book of Xen* 的作者经营的一个VPS，性价比还行，用SSH管理，支持LiveCD启动
* `Virpus <https://virpus.com/>`__: 名声不是很好，没试过


其他
----

* `VDS6 <https://vds6.net/>`__: 有OpenVZ和KVM，使用web VNC管理，可以在控制面板重装系统，没有Live CD
