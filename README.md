P2P聊天软件设计
========================================================================================

## 功能要求
* 用户界面（GUI）可自行设计，主要为用户提供方便。
* 用户界面要及时的对用户的操作做出反应。
* 要实现一个用户可以和多个用户同时进行聊天。

## 设计目标及流程
设计一个P2P聊天软件，由多个聊天客户端和一个中心服务器组成。各个聊天客户端可以两两相互发送消息。目的在于设计GUI、多线程和套接字编程。各个客户端用IP地址和TCP/IP监听端口号进行标识。每个客户提供一个名字，方便其它客户端。

此设计由两部分组成。

### 第一部分：注册
P2P聊天中各个成员管理。每个成员称为一个peer，要有一个中心的服务处理peer的登记和对已经登记的peer进行更新。 
* 步骤1：一个peer选择一个名字，通过登记界面进行登记。
* 步骤2：服务器拥有一个公有IP，在一个众所周知的端口进行监听。Peer向服务器发送‘register’请求，同时发送自己的登记信息（如己取的名字，自己的监听端口号等）。服务器收到‘register’请求后，如果这是每一个登记请求，服务器建立一个用于记录已经登记peer的列表（Peer List），否则更新Peer List。
* 步骤3：服务器向来登记的peer发送Peer List。PeerList 由一些活跃的peer组成，一个活跃的peer是在最近30秒内进行登记的peer。
* 步骤4：peer收到 Peer List后，以适当的形式显示出来。设计一个显示Peer List的GUI。
* 步骤5：每个已经登记的peer每隔15秒向服务器登记，来使自己保持活跃，同时得到最近新登记的peer信息。如果需要，就在自己的PeerList GUI进行更新。

### 第二部分：交互
多个peer间相互发送消息。
* 步骤1：在的客户端P1的用户U1（U1@P1）在自己Peer List GUI中选择一个peer，假设为U2@P2。
* 步骤2：P1建立一个到P2的连接。
* 步骤3：U1在自己的发送信息界面（Messaging GUI）编写信息，然后点击发送按钮，P1将信息通过步骤2中建立的连接发送到P2。
* 步骤4：如果第3个用户U3@P3发送一条消息到U1@P1，如果U1此时正好有一个Messaging GUI与U3进行交互，那么就显示消息，如果没有，就要提示U1有新消息到来。两个User之间可以相互发送消息，每对User之间有一个单独的Messaging GUI。

## 软件组成
本软件由ChatClient和RegisterServer两部分组成，其中ChatClient为客户端，可执行注册和聊天过程，而RegisterServer为注册服务器，主要用于注册客户端和分发注册客户端信息。

## 版本更新日志
v0.1
+ 构建出基础的软件结构，并未实现多线程（多进程）编程<使用了Qt的异步封装>
+ 通信机制还应该有所改善，不应该直接弹出聊天框，可以实现如QQ的消息提醒方式
+ 界面亟需改善

## 软件运行界面
* 客户端注册界面

![客户端注册界面](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/register_gui.jpg)

* 客户端界面

![客户端界面](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/client_gui.jpg)

![客户端界面（新消息）](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/client_gui2.jpg)

* 注册服务器界面

![注册服务器界面](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/register_server_gui.jpg)

* 客户端通信聊天界面

![客户端通信聊天界面](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/chat_dialog.jpg)

备注：由于个人不会界面设计，因此界面十分粗糙。同时由于功能的相对不完善，因此，对于整个软件的架构设计还有一些方面的不完善，后面估计会考虑去重构该软件。
