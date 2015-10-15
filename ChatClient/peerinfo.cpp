#include "peerinfo.h"

static const int TimeOut = 22 * 1000;

PeerInfo::PeerInfo(QObject *parent, QString peername, QHostAddress ip, int port) :
    QObject(parent)
{
    this->setPeerInfo(peername, ip, port);
}

PeerInfo::~PeerInfo()
{
}

QString PeerInfo::getPeerName()
{
    return this->peerName;
}

QString PeerInfo::getPeerIP()
{
    return this->peerIP.toString();
}

int PeerInfo::getPeerPort()
{
    return this->peerPort;
}

void PeerInfo::setPeerInfo(QString peername, QHostAddress ip, int port)
{
    this->peerName = peername;
    this->peerIP = ip;
    this->peerPort = port;
}

void PeerInfo::clear()
{

}
