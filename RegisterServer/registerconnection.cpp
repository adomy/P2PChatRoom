#include "registerconnection.h"

static const int TransferTimeout = 40 * 1000;
static const char SeparateToken = ' ';

RegisterConnection::RegisterConnection(QObject *parent) :
    QTcpSocket(parent)
{
    this->greetingMessage = QString("Register Validation");
    this->registerMessage = QString("");
    this->state = WaitingForGreeting;
    this->currentDataType = UNDEFINED;
    this->isGreetingMessageSent = false;
    this->numBytesForCurrentDataType = -1;
    this->transferTimerId = 0;

    connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    connect(this, SIGNAL(connected()), this, SLOT(sendGreetingMessage()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
}

RegisterConnection::~RegisterConnection()
{

}

QString RegisterConnection::getPeerServerName()
{
    return this->name;
}

QHostAddress RegisterConnection::getPeerServerIP()
{
    return this->ip;
}

int RegisterConnection::getPeerServerPort()
{
    return this->port;
}

void RegisterConnection::handleSocketError(QAbstractSocket::SocketError error)
{
    emit this->registerError(this, error);
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
        this->isGreetingMessageSent = 1;
}

bool RegisterConnection::sendPeriodicPeerlistMessage(QString message)
{
    QByteArray peerStr = message.toUtf8();
    QByteArray msg = "PEERLIST " + QByteArray::number(peerStr.size()) + ' ' + peerStr;
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

        QString greeting = QString::fromUtf8(this->buffer);

        if(DEBUG)
            qDebug() << "Get greeting message: " << greeting;

        QStringList addrList = greeting.split(":");
        this->name = addrList.at(0);
        QString ipStr = addrList.at(1);
        QString portStr = addrList.at(2);
        this->ip = QHostAddress(ipStr);
        this->port = portStr.toInt();

        if(DEBUG)
            qDebug() << "Received greeting message: " << QString::fromUtf8(this->buffer);

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
        this->registerMessage = QString::fromUtf8(this->buffer);
        emit this->newRegisterMsg(this, this->registerMessage);
        break;
    case PEERLIST:
        break;
    default:
        break;
    }

    this->currentDataType = UNDEFINED;
    this->numBytesForCurrentDataType = -1;
    this->buffer.clear();
}
