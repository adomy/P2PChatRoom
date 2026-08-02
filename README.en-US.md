

P2P Chat Software Design
========================================================================================

## Functional Requirements
* The user interface (GUI) can be designed freely, primarily to provide convenience for users.
* The GUI should respond promptly to user operations.
* The system must support a user chatting with multiple users simultaneously.

## Design Objectives and Workflow
Design a P2P chat software consisting of multiple chat clients and a central server. Each chat client can send messages to each other pairwise. The objective is to practice GUI design, multithreading, and socket programming. Each client is identified by an IP address and a TCP/IP listening port number. Each client provides a username for convenience by other clients.

This design consists of two parts.

### Part 1: Registration
Management of members in the P2P chat. Each member is referred to as a peer, and a central service is required to handle peer registration and updates for registered peers. 
* Step 1: A peer chooses a name and registers via the registration interface.
* Step 2: The server has a public IP and listens on a well-known port. The peer sends a 'register' request to the server, along with its registration information (e.g., chosen name, listening port number, etc.). Upon receiving the 'register' request, if it's the first registration request, the server creates a list to record registered peers (Peer List); otherwise, it updates the Peer List.
* Step 3: The server sends the Peer List to the registering peer. The Peer List consists of active peers, where an active peer is one that registered within the last 30 seconds.
* Step 4: After receiving the Peer List, the peer displays it in an appropriate format. Design a GUI to display the Peer List.
* Step 5: Each registered peer registers with the server every 15 seconds to keep itself active and obtain information about newly registered peers. If necessary, update its own Peer List GUI.

### Part 2: Interaction
Peers send messages to each other.
* Step 1: User U1 on client P1 (U1@P1) selects a peer, say U2@P2, from their Peer List GUI.
* Step 2: P1 establishes a connection to P2.
* Step 3: U1 composes a message in their messaging interface (Messaging GUI) and clicks the send button. P1 sends the message to P2 via the connection established in Step 2.
* Step 4: If a third user U3@P3 sends a message to U1@P1, and U1 currently has a Messaging GUI open with U3, the message is displayed. Otherwise, U1 is notified of a new incoming message. Two users can exchange messages, with a separate Messaging GUI for each pair of users.

## Software Composition
This software consists of two components: ChatClient and RegisterServer. ChatClient is the client that handles the registration and chatting processes, while RegisterServer is the registration server, primarily used for registering clients and distributing registered client information.

## Version Changelog
v0.1
+ Built the basic software structure; multithreading (multi-process) programming is not yet implemented <uses Qt's asynchronous encapsulation>
+ The communication mechanism should be improved; instead of directly popping up chat windows, it could implement a message notification style similar to QQ.
+ The interface urgently needs improvement.

## Software Screenshots
* Client Registration Interface

![客户端注册界面](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/register_gui.jpg)

* Client Interface

![客户端界面](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/client_gui.jpg)

![客户端界面（新消息）](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/client_gui2.jpg)

* Registration Server Interface

![注册服务器界面](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/register_server_gui.jpg)

* Client Chat Interface

![客户端通信聊天界面](https://raw.githubusercontent.com/adoryn/P2PChatRoom/master/Material/chat_dialog.jpg)

Note: Due to my lack of UI design skills, the interface is quite rough. Additionally, due to the relatively incomplete features, there are still some imperfections in the overall software architecture design. I plan to consider refactoring the software in the future.
