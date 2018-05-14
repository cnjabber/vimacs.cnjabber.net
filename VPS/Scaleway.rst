Scaleway
========

Scaleway 是加性价比不错的 VPS 服务提供商。今天买了个 Scaleway 巴黎机房的 VPS, 然后装了个 Arch, 体验如下：

- 内存、硬盘比同价位的其他 VPS 提供商大，CPU 偏弱
- 不支持 LiveCD, 控制台不错
- 网络： /127 的 IPv6, 速度一般，从北大校园网 ping VPS 时间为 288ms

Scaleway 的 START1-S 及更高的配置提供了 Arch 的镜像，然而最低配的 START1-XS 却没有，所以只能先用预装的镜像（我选的是 Fedora）然后再装 Arch.

注意 Scaleway 用的是 UEFI，而且似乎不支持 EFI var，所以安装 GRUB 时要使用::

  grub-install --removable --no-nvram /dev/vda

此外，为了让控制台有输出，需要在内核命令行指定 consle 为 ttyS0: ``console=tty0 console=ttyS0,9600 rd.debug rd.shell``

必须要注意的是， **VPS 启动后需要使用 Scaleway 的 API 通知 Hypervisor 虚拟机已启动** ，否则虚拟机会在启动后几分钟后自动重启，直到 Hypervisor 认为系统启动错误。请参考以下脚本和配置：

- `scw-signal-booted.service <https://github.com/scaleway/image-fedora/blob/master/28/overlay-base/etc/systemd/system/scw-signal-booted.service>`__
- `scw-signal-state <https://github.com/scaleway/image-fedora/blob/master/28/overlay-base/usr/local/sbin/scw-signal-state>`__

至于网络配置，IPv4 使用 DHCP，而 IPv6 使用静态配置，可以通过 Scaleway 的 API 获取，请参考：

- `scw-metadata <https://github.com/scaleway/image-fedora/blob/master/28/overlay-base/usr/local/bin/scw-metadata>`__
