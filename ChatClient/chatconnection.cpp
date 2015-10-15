#include "chatconnection.h"

static const int TransferTimeout = 30 * 1000;
static const char SeparatorToken = ' ';

// ChatConnection初始化和信号槽连接
ChatConnection::ChatConnection(QObject *parent) :
    QTcpSocket(parent)
{
    this->greetingMessage = QObject::tr("undefined");
    this->userName = QObject::tr("unknown");
    this->state = WaitingForGreeting;
    this->currentDataType = UNDEFINED;
    this->numBytesForCurrentDataType = -1;
    this->transferTimerId = 0;
    this->isGreetingMessageSent = false;

    connect(this, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    connect(this, SIGNAL(connected()), this, SLOT(sendGreetingMessage()));
    connect(this, SIGNAL(disconnected()), this, SLOT(handleDisconnected()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));

}

// 获取ChatConnection对端的名字
QString ChatConnection::getPeerName()
{
    return this->userName;
}

// 获取ChatConnection对端的IP地址
QString ChatConnection::getPeerIP()
{
    return this->ip.toString();
}

// 获取ChatConnection对端的端口号
int ChatConnection::getPeerPort()
{
    return this->port;
}

void ChatConnection::handleSocketError(QAbstractSocket::SocketError error)
{
    emit this->socketError(this, error);
}

void ChatConnection::handleDisconnected()
{
    emit this->disconnectedChat(this);
}

// 设置greeting消息
void ChatConnection::setGreetingMessage(QString greet)
{
    this->greetingMessage = greet;
}

// 定时器事件处理，此事件主要处理接收超时的问题（超时则关闭连接）
void ChatConnection::timerEvent(QTimerEvent *timerEvent)
{
    if(timerEvent->timerId() == this->transferTimerId)
    {
        this->abort();
        this->killTimer(transferTimerId);
        transferTimerId = 0;
    }
}

// 发送message消息
bool ChatConnection::sendMessage(const QString message)
{
    if(message.isEmpty())
        return false;

    QByteArray msg = message.toUtf8();
    QByteArray data = "MESSAGE " + QByteArray::number(msg.size()) + ' ' + msg;
    return this->write(data) == data.size();
}

// 发送greeting消息
void ChatConnection::sendGreetingMessage()
{
    QByteArray greeting = this->greetingMessage.toUtf8();
    QByteArray data = "GREETING " + QByteArray::number(greeting.size()) + ' ' + greeting;
    if( this->write(data) == data.size() )
        isGreetingMessageSent = true;
}

// 读取数据到buffer中，主要用于解析消息类型和消息长度
int ChatConnection::readDataIntoBuffer(int maxSize)
{
    if(maxSize > MaxBufferSize)
        return 0;

    int numBytesBeforeRead = this->buffer.size();

    if(numBytesBeforeRead == MaxBufferSize)
    {
        this->abort();
        return 0;
    }

    while(bytesAvailable() > 0 && buffer.size() < maxSize)
    {
        buffer.append(this->read(1));
        if(buffer.endsWith(SeparatorToken))
            break;
    }

    return buffer.size() - numBytesBeforeRead;
}

// 获取当前消息的消息长度
int ChatConnection::dataLengthForCurrentDataType()
{
    if(this->bytesAvailable() <= 0 || readDataIntoBuffer(MaxBufferSize) <= 0 || !buffer.endsWith(SeparatorToken))
        return 0;

    this->buffer.chop(1);
    int length = buffer.toInt();
    this->buffer.clear();
    return length;
}

// 解析消息的头部信息，包括消息类型和消息长度
bool ChatConnection::readProtocolHeader()
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

    if(buffer == "MESSAGE ")
        this->currentDataType = MESSAGE;
    else if(buffer == "GREETING ")
        this->currentDataType = GREETING;
    else
    {
        this->currentDataType = UNDEFINED;
        this->abort();
        return false;
    }

    buffer.clear();
    this->numBytesForCurrentDataType = this->dataLengthForCurrentDataType();
    return true;
}

bool ChatConnection::hasEnoughData()
{
    if(this->transferTimerId)
    {
        this->killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (numBytesForCurrentDataType <= 0)
        numBytesForCurrentDataType = this->dataLengthForCurrentDataType();

    if (bytesAvailable() < numBytesForCurrentDataType
            || numBytesForCurrentDataType <= 0)
    {
        this->transferTimerId = this->startTimer(TransferTimeout);
        return false;
    }

    return true;
}

// 处理socket接收消息的核心函数，根据连接状态进行不同的操作
void ChatConnection::processReadyRead()
{
    if(this->state == WaitingForGreeting)
    {
        if(!this->readProtocolHeader())
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

        this->buffer = read(this->numBytesForCurrentDataType);
        if(buffer.size() != this->numBytesForCurrentDataType)
        {
            this->abort();
            return;
        }

        QString greeting = QString::fromUtf8(this->buffer);

        if(DEBUG)
            qDebug() << "Get greeting message: " << greeting;

        QStringList addrList = greeting.split(":");
        this->userName = addrList.at(0);
        QString ipStr = addrList.at(1);
        QString portStr = addrList.at(2);
        this->ip = QHostAddress(ipStr);
        this->port = portStr.toInt();

        this->currentDataType = UNDEFINED;
        this->numBytesForCurrentDataType = 0;
        this->buffer.clear();

        if(!this->isValid())
        {
            this->abort();
            return;
        }

        if(!this->isGreetingMessageSent)
            this->sendGreetingMessage();

        this->state = ReadyForUse;
        emit this->endGreeting(this);
    }

    do
    {
        if(this->currentDataType == UNDEFINED)
        {
            if(!readProtocolHeader())
                return;
        }
        if(!this->hasEnoughData())
            return;
        this->processData();
    }while(this->bytesAvailable() > 0);
}

// 该函数用于解析接收到的消息的内容部分
void ChatConnection::processData()
{
    this->buffer = this->read(this->numBytesForCurrentDataType);
    if(buffer.size() != this->numBytesForCurrentDataType)
    {
        this->abort();
        return;
    }

    switch (currentDataType) {
    case MESSAGE:
        emit newMessage(this->userName, QString::fromUtf8(buffer));
        break;
    default:
        break;
    }

    this->currentDataType = UNDEFINED;
    this->numBytesForCurrentDataType = -1;
    this->buffer.clear();
}

