Xilinx Vivado相关软件及对FPGA远程编程
=====================================

这几天在学习用Xilinx Vivado写FPGA，了解了Xilinx Vivado相关的几个工具的区别。


Vivado Lab Solutions
--------------------

我开始接触的是这个，我把Lab Edition和Hardware Server都安装了。

Lab Edition有GUI，里面带一个Hardware Manager，其实和Vivado Design Suite里面的Hardware Manager是一样的。它可以识别出我的板子，可以让我给一个bit文件对FPGA进行编程。然而，Lab Edition并没有对HDL进行综合的功能，所以要设计硬件，还是需要装个Vivado Design Suite.

Hardware Server更小，没有GUI，它的作用也是把bit文件编程到FPGA里面，它可以开一个服务器，让远程的机器用Hardware Manager上传bit文件，之后Hardware Server可以用收到的bit文件对FPGA进行编程。


Vivado Design Suite
-------------------

这个是真正带综合功能的Vivado.

值得一提的是，它的WebPACK版本是免费的，它支持的硬件有限，但是对Digilent的板子来说，WebPACK够用了。其实WebPACK支持的芯片数量还是不少的，可能大多数用户都不需要买收费的版本。

下面讲一下在Design Suite里面远程写板，当然了，用Hardware Manager也是一样的。

首先，在跑着Hardware Server的机器上，要把FPGA板子插上机器并打开。

先打开硬件管理器(Flow->Open Hardware Manager)，点击Open target，里面有个Open New Target，在弹出的窗口里面点Next，接着看到Connect to的右边有个下拉菜单，选Remote server.接着下面有Host name，在里面填上Hardware Server所在机器的域名或者IP地址就行了。然后点Next，如果远程机器上接上了已经启动的FPGA，那么在Hardware Devices里面就会显示接上的板子，选上它就行了。之后的操作和本地编程没什么区别。

