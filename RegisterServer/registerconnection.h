/* 说明：
 * RegisterConnection用于与中心验证服务器通信，主要交换Register注册信息和PeerList活跃用户信息
 * 这里，我们也有几个设计说明：
 * (1) 我们仍然设置Greeting的过程用于初始的连接建立过程
 * (2) 我们设置周期定时器用于周期发送Register信息和获取最新的活跃用户信息
 *
 */

#ifndef REGISTERCONNECTION_H
#define REGISTERCONNECTION_H

#include "header.h"
#include <QObject>

class RegisterConnection : public QTcpSocket
{
    Q_OBJECT
public:
    enum RegisterConnectionState
    {
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };

    enum DataType
    {
        GREETING,
        REGISTER,
        PEERLIST,
        UNDEFINED
    };

    explicit RegisterConnection(QObject *parent = 0);
    ~RegisterConnection();

    QString getPeerServerName();
    QHostAddress getPeerServerIP();
    int getPeerServerPort();

signals:
    void newRegisterMsg(RegisterConnection *, QString);
    void registerError(RegisterConnection *, QAbstractSocket::SocketError);

protected:
    void timerEvent(QTimerEvent *);

public slots:
    void processReadyRead();
    void sendGreetingMessage();
    bool sendPeriodicPeerlistMessage(QString message);
    void handleSocketError(QAbstractSocket::SocketError);

private:

    // receive
    int readDataIntoBuffer(int maxSize = MaxBufferSize);
    int dataLengthForCurrentDataType();
    bool readProtocolHeader();
    bool hasEnoughData();
    void processData();

    QString name;
    QHostAddress ip;
    int port;

    QByteArray buffer;
    enum RegisterConnectionState state;
    enum DataType currentDataType;
    int numBytesForCurrentDataType;
    int transferTimerId;
    bool isGreetingMessageSent;
    QString greetingMessage;
    QString registerMessage;
};

#endif // REGISTERCONNECTION_H
