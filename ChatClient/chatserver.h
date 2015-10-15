#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "header.h"
#include "chatconnection.h"
#include <QObject>

class ChatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ChatServer(QObject *parent = 0);

    void setChatServerInfo(QHostAddress &ip, int &port);
    bool startServer();

signals:
    void newChatConnection(ChatConnection *connection);

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QHostAddress myIp;
    int myPort;
};

#endif // CHATSERVER_H
