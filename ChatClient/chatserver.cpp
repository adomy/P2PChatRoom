#include "chatserver.h"

ChatServer::ChatServer(QObject *parent) :
    QTcpServer(parent)
{
    this->myIp = QHostAddress::AnyIPv4;
    this->myPort = 9001;
}

void ChatServer::setChatServerInfo(QHostAddress &ip, int &port)
{
    this->myIp = ip;
    this->myPort = port;
}

bool ChatServer::startServer()
{
    return this->listen(this->myIp, this->myPort);
}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ChatConnection *connection = new ChatConnection(this);
    connection->setSocketDescriptor(socketDescriptor);
    emit newChatConnection(connection);
}

