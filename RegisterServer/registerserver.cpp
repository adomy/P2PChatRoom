#include "registerserver.h"

RegisterServer::RegisterServer(QObject *parent) :
    QTcpServer(parent)
{
}

void RegisterServer::setServerInfo(QHostAddress ip, int port)
{
    this->myPort = port;
    this->myIP = ip;
}

bool RegisterServer::startServer()
{
    return this->listen(this->myIP, this->myPort);
}

void RegisterServer::incomingConnection(qintptr socketDescriptor)
{
    RegisterConnection *connection = new RegisterConnection(this);
    connection->setSocketDescriptor(socketDescriptor);
    emit newRegisterConnection(connection);
}
