#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->myName = new QLabel(QObject::tr("None"), this);
    this->myAddress = new QLabel(QObject::tr("Client unregistered."), this);
    this->myIcon = new QLabel(QObject::tr("XXXXX\nXXXXX\nXXXXX"), this);
    this->myIcon->setAlignment(Qt::AlignCenter);
    this->infoLabel = new QLabel(QObject::tr("<b>Active peer list: </b>"), this);
    this->peerListWidget = new QListWidget();
    this->chatButton = new QPushButton(QObject::tr("New Chat"), this);
    this->registerButton = new QPushButton(QObject::tr("Register"), this);
    this->remoteServerLabel = new QLabel("Not set register server info.");
    this->server = new ChatServer(this);
    this->registConnection = new RegisterConnection(this);
    this->initLayout();
    this->setWindowTitle(QObject::tr("Chat Room"));
    this->resize(QSize(340, 500));
    this->setMinimumSize(QSize(280, 320));

    connect(this->chatButton, SIGNAL(clicked()), this, SLOT(handleActiveConnection()));
    connect(this->registerButton, SIGNAL(clicked()), this, SLOT(handleRegisterAction()));
}

MainWindow::~MainWindow()
{
    this->server->close();
    this->registConnection->close();
}

void MainWindow::initLayout()
{
    QVBoxLayout *myTextLayout = new QVBoxLayout;
    myTextLayout->addWidget(myName);
    myTextLayout->addWidget(myAddress);

    QHBoxLayout *myInfoLayout = new QHBoxLayout;
    myInfoLayout->addWidget(myIcon);
    myInfoLayout->addLayout(myTextLayout);
    myInfoLayout->setStretch(0, 1);
    myInfoLayout->setStretch(1, 5);

    QHBoxLayout *chatLayout = new QHBoxLayout;
    chatLayout->addWidget(registerButton);
    chatLayout->addStretch();
    chatLayout->addWidget(chatButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(myInfoLayout);
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(peerListWidget);
    mainLayout->addLayout(chatLayout);
    mainLayout->addWidget(remoteServerLabel);

    QWidget *widget = new QWidget(this);
    widget->setLayout(mainLayout);

    this->setCentralWidget(widget);
}

// 网络模块启动部分，包括启动本地的聊天服务器，以及连接至注册服务器
void MainWindow::initNetwork()
{
    // 启动ChatServer
    if(DEBUG)
    {
        qDebug() << "Local server ip address: " << this->localServerIP.toString();
        qDebug() << "Local server port: " << this->localServerPort;
    }

    this->server->setChatServerInfo(this->localServerIP, this->localServerPort);
    if(this->server->startServer())
    {
        QString address = this->localServerIP.toString() + ":" + QString::number(this->localServerPort);
        this->myName->setText(this->userName);
        this->myAddress->setText(address);

        QString remoteAddress = QString("Register Server Info: ") + this->remoteServerIP.toString() + ":" + QString::number(this->remoteServerPort);
        this->remoteServerLabel->setText(remoteAddress);

        this->registerButton->setEnabled(false);
        if(DEBUG)
            qDebug() << "Local chat server started.";
    }
    else
    {
        this->remoteServerLabel->setText("Cannot start local chat server.");
        this->registerButton->setEnabled(true);
        QMessageBox::warning(this, tr("Error"), tr("Cannot start local chat server!"));
        if(DEBUG)
            qDebug() << "Cannot start local chat server.";
        return;
    }
    connect(server, SIGNAL(newChatConnection(ChatConnection*)), this, SLOT(handlePassiveConnection(ChatConnection*)));

    // 连接至中心注册服务器
    if(DEBUG)
        qDebug() << "Connect to register server.";
    this->registConnection->setLocalServerInfo(this->userName, this->localServerIP, this->localServerPort);
    this->registConnection->connectToHost(this->remoteServerIP, this->remoteServerPort);
    if(DEBUG)
        qDebug() << "Connected to register server.";
    connect(registConnection, SIGNAL(newPeerList(QString&)), this, SLOT(handleNewPeerList(QString&)));
    connect(registConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleRegisterError(QAbstractSocket::SocketError)));
}

void MainWindow::handleRegisterAction()
{
    LoginDialog *ld = new LoginDialog(this);
    if (ld->exec() != QDialog::Accepted)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Client Register Canceled!"));
        return;
    }

    this->userName = ld->getUsername();
    this->localServerIP = ld->getLocalServerIP();
    this->localServerPort = ld->getLocalServerPort();
    this->remoteServerIP = ld->getRegisterServerIP();
    this->remoteServerPort = ld->getRegisterServerPort();

    if(DEBUG)
    {
        qDebug() << "Register client configuration done!";
        qDebug() << "Local server name: " << this->userName;
        qDebug() << "Local server ip address: " << this->localServerIP;
        qDebug() << "Local server port: " << this->localServerPort;
    }

    this->initNetwork();
}

void MainWindow::updateListWidget()
{
    this->peerListWidget->clear();
    for(int i = 0; i < this->peerList.size(); i++)
    {
        PeerInfo *pi = this->peerList.at(i);
        QString itemString = pi->getPeerName() + "@" + pi->getPeerIP() + ":" + QString::number(pi->getPeerPort());
        this->peerListWidget->addItem(new QListWidgetItem(itemString));
    }
}

void MainWindow::handleNewPeerList(QString &peerListString)
{
    this->mutex1.lock();

    this->peerList.clear();
    if(peerListString.isEmpty())
        return;

    QString username;
    QHostAddress ip;
    int port;
    QStringList peerInfoList = peerListString.split(";");
    foreach(QString peerInfo, peerInfoList)
    {
        QStringList items = peerInfo.split("@");
        username = items.at(0);
        QString addr = items.at(1);
        QStringList addrList = addr.split(":");
        QString ipString = addrList.at(0);
        ip = QHostAddress(ipString);
        QString portString = addrList.at(1);
        port = portString.toInt();
        PeerInfo *pinfo = new PeerInfo(this, username, ip, port);
        this->peerList.push_back(pinfo);
    }
    this->mutex1.unlock();

    this->updateListWidget();
}

void MainWindow::handlePassiveConnection(ChatConnection *connection)
{
    if(DEBUG)
        qDebug() << "New passive chat connection.";

    QString greeting = this->userName + ":" + this->localServerIP.toString() + ":" + QString::number(this->localServerPort);
    connection->setGreetingMessage(greeting);

    this->addChatConnection(connection);

    connect(connection, SIGNAL(socketError(ChatConnection*,QAbstractSocket::SocketError)),
            this, SLOT(handleChatConnectionError(ChatConnection*,QAbstractSocket::SocketError)));
    connect(connection, SIGNAL(endGreeting(ChatConnection*)), this, SLOT(handleNewChatDialog(ChatConnection*)));
}

void MainWindow::handleActiveConnection()
{
    if(DEBUG)
        qDebug() << "New active chat connection.";

    QListWidgetItem *item = this->peerListWidget->currentItem();
    if(!item)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please select one peer."));

        if(DEBUG)
            qDebug() << "Select no peer to connect.";
        return;
    }

    QString connInfo = item->text();
    QStringList strList = connInfo.split("@");
    QString name = strList.at(0);
    QString addr = strList.at(1);
    QStringList addrList = addr.split(":");
    QString ipStr = addrList.at(0);
    QString portStr = addrList.at(1);
    QHostAddress ip = QHostAddress(ipStr);
    int port = portStr.toInt();

    if(name == this->userName && ipStr == this->localServerIP.toString() && port == this->localServerPort)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot select local server."));

        if(DEBUG)
            qDebug() << "Select local server to connect.";

        return;
    }

    if(DEBUG)
    {
        qDebug() << "Establish chat connection to selected peer.";
        qDebug() << "Remote peer ip: " << ip.toString();
        qDebug() << "Remote peer port: " << port;
    }

    ChatConnection *newConnection = new ChatConnection(this);
    QString greeting = this->userName + ":" + this->localServerIP.toString() + ":" + QString::number(this->localServerPort);
    newConnection->setGreetingMessage(greeting);
    newConnection->connectToHost(ip, port);
    this->addChatConnection(newConnection);
    connect(newConnection, SIGNAL(socketError(ChatConnection*, QAbstractSocket::SocketError)),
            this, SLOT(handleChatConnectionError(ChatConnection*, QAbstractSocket::SocketError)));
    connect(newConnection, SIGNAL(endGreeting(ChatConnection*)), this, SLOT(handleNewChatDialog(ChatConnection*)));

    if(DEBUG)
        qDebug() << "Open new chat dialog to display chat process.";
}

void MainWindow::handleNewChatDialog(ChatConnection *connection)
{
    QString peerName = connection->getPeerName();
    QString peerAddr = connection->getPeerIP() + ":" + QString::number(connection->getPeerPort());

    ChatDialog *newChat = new ChatDialog(this, this->userName, peerName, peerAddr, connection);
    newChat->show();
}

void MainWindow::handleChatConnectionError(ChatConnection *connection, QAbstractSocket::SocketError error)
{
    if(DEBUG)
        qDebug() << "Chat connection Error.";

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
    connection->close();
    this->removeChatConnection(connection);

    if(DEBUG)
    {
        qDebug() << "Chat connection error: " << errorString;
        qDebug() << "Handle chat connection error done.";
    }
}

void MainWindow::handleRegisterError(QAbstractSocket::SocketError error)
{
    if(DEBUG)
        qDebug() << "Register network error.";

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
    this->server->close();
    this->registConnection->close();
    this->peerList.clear();
    this->peerListWidget->clear();
    this->registerButton->setEnabled(true);
    this->remoteServerLabel->setText("Not set register server info.");

    QMessageBox::warning(this, tr("Register Error"), errorString);

    if(DEBUG)
        qDebug() << "Handle register network error done.";
}

void MainWindow::addChatConnection(ChatConnection *connection)
{
    this->mutex2.lock();
    this->connList.push_back(connection);
    this->mutex2.unlock();

    if(DEBUG)
        qDebug() << "Add new chat connection to connection list.";
}

void MainWindow::removeChatConnection(ChatConnection *connection)
{
    this->mutex2.lock();
    this->connList.removeOne(connection);
    this->mutex2.unlock();

    if(DEBUG)
        qDebug() << "Remove chat connection from connection list";
}
