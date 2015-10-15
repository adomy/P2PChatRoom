#include "peerinfo.h"

static const int TimeOut = 22 * 1000;

PeerInfo::PeerInfo(QObject *parent, QString peername, QHostAddress ip, int port) :
    QObject(parent)
{
    this->setPeerInfo(peername, ip, port);
    this->state = true;
    this->activeTimer = new QTimer();
    this->activeTimer->setInterval(TimeOut);
    this->activeTimer->start();

    if(DEBUG)
        qDebug() << "PeerInfo active timer started.";
    connect(activeTimer, SIGNAL(timeout()), this, SLOT(handleTimeOut()));
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

void PeerInfo::handleTimeOut()
{
    this->state = false;
    this->activeTimer->stop();
    emit this->peerExpire(this);
}

void PeerInfo::refresh()
{
    if(this->state == false)
        this->state = true;

    this->activeTimer->setInterval(TimeOut);
    this->activeTimer->start();
}

void PeerInfo::clear()
{
    this->state = true;
    this->activeTimer->stop();
}


