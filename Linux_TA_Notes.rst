操作系统实习课程助教笔记
========================

助教联系方式
------------

* 地点: 理科一号楼 1817
* 邮箱: vimacs.hacks AT gmail.com

 - OpenPGP fingerprint: 9E5B 817B FF33 8DD7 0167  6C27 6DBD 8BFE 8600 BEAA

* 电话: use **printf "%d\n" 0x46131b3be** to get my phone number
* GitHub: https://github.com/mytbk/
* 未名BBS: vimacs
* `Matrix <https://matrix.org>`_: ``@vimacs:matrixim.cc``
* `XMPP <https://xmpp.org>`_: ``vimacs@cnjabber.net``

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

#. **pr_debug()** 输出的信息在 dmesg 中不显示，原因是 **pr_debug** 只有在 **DEBUG** 宏打开或者打开了 **ENABLE_DYNAMIC_DEBUG** 选项时才起作用。 **Documentation/CodingStyle** 中有说明，*include/linux/printk.h* 中也能看到相关宏定义。

#. 自己编译的内核无法启动系统: 如果在 ``make install`` 的时候发现 initramfs-tools 提示 WARNING 说找不到模块，应当先执行 ``make modules_install``.

   - ``make install`` 的功能仅仅是调用系统中的 ``/sbin/installkernel``

#. 要在内核版本中显示自定义的名字，可以修改 EXTRAVERSION 和使用 config 中的 LOCALVERSION，其中 EXTRAVERSION 在内核的 Makefile 中修改，LOCALVERSION 在以下位置配置:

   ::

     General setup --->
       ()  Local version - append to kernel release

   最终在内核版本后显示的字符串是 $(EXTRAVERSION)$(LOCALVERSION)

第 5 次课
---------

本次课讲解抽象层次，内容比较杂，介绍了内核中的分层架构、API 和 ABI 的区别、系统调用和VDSO等。

实习题目:

#. 修改 LKMPG 8.1 的例子，替换 ``open`` 调用，截获 test 用户的操作。

#. 构建动态链接的 `hello world` 程序，用 ``strace`` 获取其系统调用。这些调用做了什么功能？

检查作业时遇到的问题:

#. 修改系统调用表的方式:

   - 导出 ``sys_call_table`` 后编译内核
   - 加载模块前看 ``/proc/kallsyms`` 找 ``sys_call_table`` 的地址，然后让模块知道系统调用表的地址，注意内核可能会在启动时随机化这个地址
   - 模块加载时搜索系统调用表，参考 `<https://github.com/leurfete/simple-rootkit/>`_
   - 用 ``kallsyms_lookup_name``

#. 注意学会用 ``man`` 查看函数的使用方式，包括系统调用

第 6 次课
---------

这次课讲解 Linux 源码中的头文件。

#. 选择 `lib/` 下的一个源文件，给出它的头文件包含关系图。

#. 用 ``make headerdep`` 检查头文件嵌套包含，并解决一个嵌套包含关系。

提示：学会使用 ``scripts/headerdep.pl``. 还有，Linux 源码会使用 libgcc 和 glibc 的头文件。

第 7 次课
---------

本次课的名字叫调试之道。

调试中的概念：

* bug, typo, bug tracking system, Heisenbug
* remote debugging
* post-mortem, coredump
* "wolf fence" debugging using ``git bisect``

实习题目：

#. 使用 **print_hex_dump** 打印 **boot_command_line** 和 **saved_command_line**

#. 学会使用 Magic SysRQ

#. 学会使用 **dump_stack**

#. 学习 `Debugging the kernel using Ftrace <https://lwn.net/Articles/365835/>`_
