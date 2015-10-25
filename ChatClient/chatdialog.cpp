#include "chatdialog.h"

ChatDialog::ChatDialog(QWidget *parent, QString username, QString peername, QString peeraddr, ChatConnection *connection) :
    QDialog(parent)
{
    this->userName = username;
    this->chatSession = connection;

    if(DEBUG)
        qDebug() << "Peer addr string is: " << peeraddr;

    //this->iconLabel = new QLabel(QObject::tr("XXXXX\nXXXXX\nXXXXX"), this);
    //this->iconLabel->setAlignment(Qt::AlignCenter);
    this->peerName = new QLabel(this);
    this->peerAddress = new QLabel(this);
    this->logChat = new QTextEdit();
    this->msgSend = new QLineEdit();
    this->sendButton = new QPushButton(QObject::tr("Send"), this);
    this->msgSend->setFocusPolicy(Qt::StrongFocus);
    this->logChat->setFocusPolicy(Qt::NoFocus);
    this->logChat->setReadOnly(true);

    this->peerName->setText("Username: " + peername);
    this->peerAddress->setText("Address: " + peeraddr);
    this->setWindowTitle(peername + "@" + peeraddr);
    this->initLayout();
    this->resize(QSize(480, 360));

    connect(sendButton, SIGNAL(clicked()), this, SLOT(handleToSendMessage()));
    connect(msgSend, SIGNAL(returnPressed()), this, SLOT(handleToSendMessage()));

    connect(chatSession, SIGNAL(newMessage(QString, QString)), this, SLOT(handleReceivedMssage(QString, QString)));
    connect(chatSession, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleConnectionError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));
}

ChatDialog::~ChatDialog()
{

}

void ChatDialog::initLayout()
{
    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(peerName);
    infoLayout->addWidget(peerAddress);

    /****************************************
    QHBoxLayout *peerLayout = new QHBoxLayout;
    peerLayout->addWidget(iconLabel);
    peerLayout->addLayout(infoLayout);
    peerLayout->setStretch(0, 1);
    peerLayout->setStretch(1, 6);
    ****************************************/

    QHBoxLayout *sendLayout = new QHBoxLayout;
    sendLayout->addWidget(msgSend);
    sendLayout->addWidget(sendButton);
    sendLayout->setStretch(0, 5);
    sendLayout->setStretch(1, 1);

    QVBoxLayout *dialogLayout = new QVBoxLayout;
    //dialogLayout->addLayout(peerLayout);
    dialogLayout->addLayout(infoLayout);
    dialogLayout->addWidget(logChat);
    dialogLayout->addLayout(sendLayout);
    dialogLayout->setStretch(0, 1);
    dialogLayout->setStretch(1, 8);
    dialogLayout->setStretch(2, 2);

    this->setLayout(dialogLayout);
}

void ChatDialog::handleReceivedMssage(QString from, QString message)
{
    if (from.isEmpty() || message.isEmpty())
        return;

    QDateTime time = QDateTime::currentDateTime();
    QString timeString = time.toString("hh:mm:ss MM-dd");
    QString infoString =  from + "(" + timeString + "): ";
    QColor color = this->logChat->textColor();
    this->logChat->setTextColor(Qt::blue);
    this->logChat->append(infoString);
    this->logChat->append(message + "\n");
    this->logChat->setTextColor(color);
    QScrollBar *bar = logChat->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void ChatDialog::readAndDisplayHistory(QList<QString> strList)
{
    QColor color = this->logChat->textColor();
    this->logChat->setTextColor(Qt::blue);
    foreach(QString s, strList)
        this->logChat->append(s);
    this->logChat->setTextColor(color);
    QScrollBar *bar = logChat->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void ChatDialog::handleToSendMessage()
{
    QString text = this->msgSend->text();
    if (text.isEmpty())
        return;

    QColor color = logChat->textColor();
    if (text.startsWith(QChar('/'))) {
        this->logChat->setTextColor(Qt::black);
        this->logChat->append(tr("Unknown command: %1")
                         .arg(text.left(text.indexOf(' '))));
    }
    else
    {
        if(!chatSession->sendMessage(text))
        {
            this->logChat->setTextColor(Qt::red);
            this->logChat->append("Transfer Error: cannot send message: " + text);
            this->msgSend->clear();
            this->logChat->setTextColor(color);
            QScrollBar *bar = logChat->verticalScrollBar();
            bar->setValue(bar->maximum());

            if(DEBUG)
                qDebug() << "Error send message.";
            return;
        }

        QDateTime time = QDateTime::currentDateTime();
        QString timeString = time.toString("hh:mm:ss MM-dd");
        QString infoString = this->userName + "(" + timeString + ") : ";

        QColor color = this->logChat->textColor();
        this->logChat->setTextColor(Qt::black);
        this->logChat->append(infoString);
        this->logChat->append(text + "\n");
        this->logChat->setTextColor(color);

        QScrollBar *bar = logChat->verticalScrollBar();
        bar->setValue(bar->maximum());
    }

    this->msgSend->clear();
    this->logChat->setTextColor(color);

    if(DEBUG)
        qDebug() << "Send Message Success" << endl;
}

void ChatDialog::handleConnectionError(QAbstractSocket::SocketError error)
{
    this->chatSession->close();
    QString errorString;
    switch(error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        errorString = "Connection refused!";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorString = "Remote host closed.";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorString = "Host not found!";
        break;
    case QAbstractSocket::NetworkError:
        errorString = "Network error!";
        break;
    case QAbstractSocket::AddressInUseError:
        errorString = "Address already in use!";
        break;
    case QAbstractSocket::UnknownSocketError:
    default:
        errorString = "Unknown socket error!";
        break;
    }
    QColor color = this->logChat->textColor();
    this->logChat->setTextColor(Qt::red);
    this->logChat->append("Connection Error:" + errorString);
    this->logChat->setTextColor(color);
}

void ChatDialog::handleRejected()
{
    emit this->dialogExit(this->chatSession);
    this->chatSession->close();
}
