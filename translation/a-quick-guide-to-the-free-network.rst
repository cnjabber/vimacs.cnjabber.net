自由网络快速指南
================

* 原文：https://medium.com/we-distribute/a-quick-guide-to-the-free-network-c069309f334
* 作者：Sean Tilley
* 翻译：vimacs

(本文以 `署名-相同方式共享 4.0 国际 (CC BY-SA 4.0) <https://creativecommons.org/licenses/by-sa/4.0/>`_ 协议发布。)


很多到联邦网络来的新人可能听过对 fediverse 不经意的提及，并且可能会好奇这个词是什么意思。一个简单的解释是它是 "federation" 和 "universe" 的混成词，但事实上更加复杂。让我们深入了解这个野兽的本性，以及一些和它一起过来的历史。

现在，在联邦的社交空间中有两个超级网络，它们跑着不同的协议。它们被称为 The Fediverse 和 The Federation. 这两个超级网络以相似的方式甚至是相同的目的工作，然而它们有着不同的开发历史，引申开来，有着不同的协议栈。这重叠的一系列网络可以宏观地称为自由网络。

  .. image:: https://cdn-images-1.medium.com/max/1600/1*UCQHwil3f0av0AWmafmdrw.png

  8个不同的平台，互相独立地开发。Hubzilla 和 Friendica 可以和两个网络交谈，因为这些项目是协议无关并且可以通过插件扩展。

什么是 Fediverse?
~~~~~~~~~~~~~~~~~

历史上，Fediverse 是一个微博网络，并使用 Ostatus 协议让服务器互相沟通。总共有六个不同的平台：GNU Social, postActiv, Pleroma, Mastodon, Friendica, 和 Hubzilla.

Fediverse 最初由少数都跑在 StatusNet 平台，可以随便地描述为具有特殊的为群组设计的交流特性的类似 Twitter 的平台，的服务器建立。因为它的微博特性，文章和评论都被认为是同种对象，称为状态。

StatusNet 最终称为 GNU Social 项目的中心，它的开发一直在以一个平稳的步伐继续。它被分支到 postActiv 项目，它的目标是清理系统的后端和用户界面。Mastodon 最初开发为一个基于 Ruby on Rails 的 OStatus 实现，并且也可以连接到其他的这些网络。最后，Pleroma 项目以 GNU Social 的另一个前端开始，但最终有了它自己用 Elixir 写的后端。


什么是 Federation?
~~~~~~~~~~~~~~~~~~~~~~~~~~

Federation 是一个由 278 个不同的用 Diaspora 联邦协议交流的联通的服务器组成的互操作的网络。这是一个和 OStatus 不同的通信标准，并允许四个不同的平台互相通信：Diaspora, Friendica, Hubzilla, 和 Socialhome.

Federation 从 2010 年由只运行 Diaspora 的服务器开始。从结构上来说，Diaspora 在工作上更像 Facebook：它支持长式而不是短式的内容，并且每个文章有指定的评论线。它还支持私人状态和一个用于私信的收件箱。

在 2012 年，Friendica 项目通过逆向工程 Diaspora 通信协议并从头编写一个 PHP 实现库创立，允许 Friendica 用户和 Diaspora 用户互相交流。这个工作最终移植到了 Hubzilla，一个拥有云存储和身份提供能力的内容管理系统。

在 2016 年初，Jason Robinson，一个前 Diaspora 项目的志愿贡献者，发布了 Socialhome. 因为这个平台用了 Django 而不是 Rails，Jason 不得不从头写自己的基于 Python 的联邦程序库。现在，Socialhome 在它自身的开发历史还很早，最新的版本是 0.4.0.


未来？
~~~~~~

现在，空间上的几个项目正在接纳新的补充协议，想要建立相互之间更好的桥梁。计划的开发可能最终看起来是这样：

  .. image:: https://cdn-images-1.medium.com/max/1600/1*3pEK-Fwq7bNOVcnXfVdNuQ.png

当前 Diaspora 没有新协议的计划，一直在显著地升级自己的协议。postActiv 想要在以后的版本中收入 Diaspora 联邦的支持。Mastodon 刚刚发布了对 ActivityPub 的支持，而 Pleroma、Socialhome 和 GNU Social 正在考虑收入它。Nextcloud 也特别地进入了联邦空间，而 Hubzilla 和 Friendica 很可能都会以扩展的方式支持 ActivityPub 协议。

  .. image:: https://cdn-images-1.medium.com/max/1600/1*rEa3EGO1v1vC90E1t82UGA.png

过段时间，这些不同的超级网络可能会调入一个包含每个人的联邦的超级网络，最大化九个，可能更多个不同系统之间的互操作性。虽然这样一种情形还需要很长时间，但是我们看到所有这些空间中主要的项目互联还是完全可能的。