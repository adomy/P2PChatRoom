#include "registerconnection.h"

static const int TransferTimeout = 40 * 1000;
static const int PeriodicInterval = 15 * 1000;
static const char SeparateToken = ' ';

RegisterConnection::RegisterConnection(QObject *parent) :
    QTcpSocket(parent)
{
    this->greetingMessage = QObject::tr("Register Validation");
    this->registerMessage = QObject::tr(" ");
    this->state = WaitingForGreeting;
    this->currentDataType = UNDEFINED;
    this->isGreetingMessageSent = false;
    this->numBytesForCurrentDataType = -1;
    this->transferTimerId = 0;
    this->periodicTimer.setInterval(PeriodicInterval);

    connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    connect(&periodicTimer, SIGNAL(timeout()), this, SLOT(sendPeriodicRegisterMessage()));
    connect(this, SIGNAL(connected()), this, SLOT(sendGreetingMessage()));
}

void RegisterConnection::setLocalServerInfo(QString &name, QHostAddress &ip, int &port)
{
    this->localUserName = name;
    this->localServerIP = ip;
    this->localServerPort = port;
    this->registerMessage = this->localUserName + '@' +
            this->localServerIP.toString() + ':' +
            QString::number(this->localServerPort);
    this->greetingMessage = this->localUserName + ':' +
            this->localServerIP.toString() + ':' +
            QString::number(this->localServerPort);
}

void RegisterConnection::timerEvent(QTimerEvent *timerEvent)
{
    if(timerEvent->timerId() == this->transferTimerId)
    {
        this->abort();
        this->killTimer(transferTimerId);
        transferTimerId = 0;
    }
}

void RegisterConnection::sendGreetingMessage()
{
    QByteArray greet = this->greetingMessage.toUtf8();
    QByteArray msg = "GREETING " + QByteArray::number(greet.size()) + ' ' + greet;
    if(this->write(msg) == msg.size())
    {
        if(DEBUG)
            qDebug() << "Send greeting message!";
        this->isGreetingMessageSent = 1;

        this->sendPeriodicRegisterMessage();
        this->periodicTimer.start();
        if(DEBUG)
            qDebug() << "Send register message!";
    }
}

bool RegisterConnection::sendPeriodicRegisterMessage()
{
    QByteArray regist = this->registerMessage.toUtf8();
    QByteArray msg = "REGISTER " + QByteArray::number(regist.size()) + ' ' + regist;
    return this->write(msg) == msg.size();
}

int RegisterConnection::readDataIntoBuffer(int maxSize)
{
    if(maxSize > MaxBufferSize)
        return 0;

    int numBytesBeforeRead = this->buffer.size();
    if(numBytesBeforeRead >= MaxBufferSize)
    {
        this->abort();
        return 0;
    }

    while(this->bytesAvailable() > 0 && buffer.size() <= maxSize)
    {
        this->buffer.append(this->read(1));
        if(buffer.endsWith(SeparateToken))
            break;
    }

    return buffer.size() - numBytesBeforeRead;
}

int RegisterConnection::dataLengthForCurrentDataType()
{
    if(this->bytesAvailable() <=0 || this->readDataIntoBuffer() <= 0 || !this->buffer.endsWith(SeparateToken))
        return 0;

    this->buffer.chop(1);
    int length = this->buffer.toInt();
    this->buffer.clear();

    return length;
}

bool RegisterConnection::hasEnoughData()
{
    if(this->transferTimerId != 0)
    {
        this->killTimer(this->transferTimerId);
        this->transferTimerId = 0;
    }

    if(this->numBytesForCurrentDataType <= 0)
        this->numBytesForCurrentDataType = this->dataLengthForCurrentDataType();

    if(bytesAvailable() < this->numBytesForCurrentDataType || this->numBytesForCurrentDataType <= 0)
    {
        this->transferTimerId = this->startTimer(TransferTimeout);
        return false;
    }

    return true;
}

void RegisterConnection::processReadyRead()
{
    if(this->state == WaitingForGreeting)
    {
        if(!readProtocolHeader())
            return;
        if(this->currentDataType != GREETING)
        {
            this->abort();
            return;
        }

        this->state = ReadingGreeting;
    }

    if(this->state == ReadingGreeting)
    {
        if(!this->hasEnoughData())
            return;

        this->buffer = this->read(this->numBytesForCurrentDataType);
        if(this->buffer.size() != this->numBytesForCurrentDataType)
        {
            this->abort();
            return;
        }

        this->buffer.clear();
        this->numBytesForCurrentDataType = -1;
        this->currentDataType = UNDEFINED;

        if(!this->isValid())
        {
            this->abort();
            return;
        }

        if(!this->isGreetingMessageSent)
            this->sendGreetingMessage();

        this->state = ReadyForUse;
    }

    do
    {
        if(currentDataType == UNDEFINED)
        {
            if(!readProtocolHeader())
                return;
        }

        if(!this->hasEnoughData())
            return;

        this->processData();
    }while(this->bytesAvailable() > 0);
}

bool RegisterConnection::readProtocolHeader()
{
    if(this->transferTimerId != 0)
    {
        this->killTimer(this->transferTimerId);
        this->transferTimerId = 0;
    }

    if(this->readDataIntoBuffer() <= 0)
    {
        this->transferTimerId = this->startTimer(TransferTimeout);
        return false;
    }

    if(this->buffer == "GREETING ")
        this->currentDataType = GREETING;
    else if (this->buffer == "REGISTER ")
        this->currentDataType = REGISTER;
    else if (this->buffer == "PEERLIST ")
        this->currentDataType = PEERLIST;
    else
    {
        this->currentDataType = UNDEFINED;
        this->abort();
        return false;
    }

    this->buffer.clear();
    this->numBytesForCurrentDataType = this->dataLengthForCurrentDataType();
    return true;
}

void RegisterConnection::processData()
{
    this->buffer = this->read(this->numBytesForCurrentDataType);
    if(this->buffer.size() != this->numBytesForCurrentDataType)
    {
        this->abort();
        return;
    }

    switch(this->currentDataType)
    {
    case REGISTER:
        break;
    case PEERLIST:
        this->peerListString = QString::fromUtf8(this->buffer);
        emit this->newPeerList(this->peerListString);
        break;
    default:
        break;
    }

    this->currentDataType = UNDEFINED;
    this->numBytesForCurrentDataType = -1;
    this->buffer.clear();
}
