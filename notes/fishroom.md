fishroom消息处理过程
====================

为了搞清tuna/fishroom是怎么转发消息的，我看了一下它的消息处理过程。

fishroom主程序
--------------

主程序为fishroom.py. fishroom主程序定义两个消息总线。

```
redis_client = get_redis()
msgs_from_im = MessageBus(redis_client, MsgDirection.im2fish)
msgs_to_im = MessageBus(redis_client, MsgDirection.fish2im)
```

fishroom不断从``msgs_from_im``中获取消息，检查binding看获取的消息是否是发往fishroom要处理的房间的。对于过长的消息，则会将上传到``text_store``，并将它的地址存放至``msg.opt['text_url']``.

最后这条消息会发往``msgs_to_im``消息总线。

IM转发
-------

每个IM对应一个Python程序，它们也都使用两个消息总线，同时开两个线程分别操作两个总线。

在处理来自fishroom的消息时，会使用``forward_msg_from_fishroom``这个过程，它在base.py中定义。

+ 如果这个IM支持发送图片，则会调用它的``send_photo``方法。
+ 否则则调用``send_msg``，注意它有个raw参数，也就是说msg结构还是会传到``send_msg``方法中，``msg.opt``也是``send_msg``的方法之一。

