操作系统实习课程助教笔记
========================

助教联系方式
------------

* 地点: 理科一号楼 1817
* 邮箱: vimacs.hacks AT gmail.com

  * OpenPGP fingerprint: 9E5B 817B FF33 8DD7 0167  6C27 6DBD 8BFE 8600 BEAA
* 电话: use **printf "%d\n" 0x46131b3be** to get my phone number
* GitHub: https://github.com/mytbk/
* 未名BBS: vimacs
* `Matrix <https://matrix.org>`_: @vimacs:matrixim.cc
* `XMPP <https://xmpp.org>`_: vimacs@cnjabber.net

第 1 次课
---------

第一次课的内容是搭建环境。视频放在 https://lcpu.club/file/media/os-video/

实习题目：

1. 在虚拟机中安装 Linux 操作系统（这次课用的是 Ubuntu 16.04 LTS,带Linux 4.4 LTS 内核）
2. 使用 SSH 访问虚拟机
3. 学会设置系统软件源
4. 搭建开发环境
5. 学会用 `Vim <http://www.vim.org/>`_ 编辑器，用 taglist 插件查看代码中的符号
6. 学会用共享目录在宿主机和虚拟机之间共享文件

第 2 次课
---------

第二次课讲了如何用 git 获得 Linux 内核源码。

实习题目：

1. 建立 GitHub 帐号，fork `torvalds/linux <https://github.com/torvalds/linux>`_ 仓库
2. 用 git 把 Linux 内核源码以裸库的形式克隆到本地，并 checkout 出一份源码

得到源码之后，可以用 **git checkout v4.4** 获得 Linux 4.4 版本的源码。

第 3 次课
---------

第三次课讲的是 Linux 系统的启动流程，读了 Linux 内核启动初期的相关源码。

实习题目：

1. 使用内核命令行参数 **initcall_debug**  (见 Linux 源码 **Documentation/kernel-parameters.txt**) 启动系统
2. 用 Linux 源码中的 *scripts/bootgraph.pl* 生成一个 SVG 图片
3. 根据生成的 SVG 图片列出时间最长的10个 initcall，并查阅它们的功能

第 4 次课
---------

这星期讲解了如何构建一个可以运行的内核，内核模块的作用以及代码风格。

实习题目：

1. 构建并更新一个内核，LOCALVERSION 为自己名字的缩写，用这个内核启动系统
2. 阅读 TLDP 上的 `The Linux Kernel Module Programming Guide <http://www.tldp.org/LDP/lkmpg/2.6/html>`_,完成 hello-1.c 到 hello-5.c 这5个模块
3. 阅读 **Documentation/CodingStyle** 总结内核的代码风格
4. 用 *scripts/checkpatch.pl* 检查自己写的代码

作业检查时遇到的问题：

1. **pr_debug()** 输出的信息在 dmesg 中不显示，原因是 **pr_debug** 只有在 **DEBUG** 宏打开或者打开了 **ENABLE_DYNAMIC_DEBUG** 选项时才起作用。 **Documentation/CodingStyle** 中有说明，*include/linux/printk.h* 中也能看到相关宏定义。
2. 自己编译的内核无法启动系统，能用 emergency 模式：原因是安装系统的时候使用 LVM 安装，initramfs 缺少驱动。
