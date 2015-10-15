/* 说明：
 * 对于整个ChatConnection来说，其主要功能是提供用于建立聊天连接和进行聊天交互的信息接收与发送过程
 * 有几个设计性说明：
 * (1) ChatConnectionState 标识当前连接的状态，分别为等待greeting、读取greeting和读取聊天信息
 * (2) DataType 标识接收信息的类型，主要有greeting信息和message信息以及未定义的信息（错误）
 * 对于其结构来说，主要分为下面几个方面：
 * 1. 我们设置Greeting信息用于最初的连接建立时的连通性检测
 *   peerHost <--->Greeting<---> peerHost
 *   当连接建立成功之后（发送connected信号），便发送greeting消息；同时，连接建立成功之后，连接工作在等待greeting的状态
 * 2. 对于消息的传递，我们设置了"消息类型 消息长度 消息内容"的传输格式，通过空格键相隔离
 *   因此，当我们解析消息时，首先应该去解析消息类型，然后读取消息长度，最后根据消息长度读取消息的内容
 * 3. 另外，对于ChatConnection，我们通过peer端的userName去标识该连接
 */

#ifndef CHATCONNECTION_H
#define CHATCONNECTION_H

#include "header.h"
#include <QObject>

class ChatConnection : public QTcpSocket
{
    Q_OBJECT
public:
    explicit ChatConnection(QObject *parent = 0);

    enum ChatConnectionState{
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };

    enum DataType{
        MESSAGE,
        GREETING,
        UNDEFINED
    };

    QString getPeerName();
    QString getPeerIP();
    int getPeerPort();
    void setGreetingMessage(QString message);
    bool sendMessage(QString message);

signals:
    void endGreeting(ChatConnection *);
    void newMessage(QString from, QString message);
    void disconnectedChat(ChatConnection *);
    void socketError(ChatConnection *, QAbstractSocket::SocketError);

protected:
    void timerEvent(QTimerEvent *);

private slots:
    void processReadyRead();
    void sendGreetingMessage();
    void handleDisconnected();
    void handleSocketError(QAbstractSocket::SocketError);

private:
    int readDataIntoBuffer(int maxSize = MaxBufferSize);
    int dataLengthForCurrentDataType();
    bool readProtocolHeader();
    bool hasEnoughData();
    void processData();

    QString userName; //通过远端的信息来标识该连接
    QHostAddress ip;
    int port;
    QString greetingMessage;
    QByteArray buffer;
    enum ChatConnectionState state;
    enum DataType currentDataType;
    int numBytesForCurrentDataType;
    int transferTimerId;
    bool isGreetingMessageSent;
};

#endif // CHATCONNECTION_H
