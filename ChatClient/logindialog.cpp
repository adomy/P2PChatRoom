#include "logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    // Initialization Parts
    this->loginLogo = new QLabel(QObject::tr("Register client"), this);
    this->loginLogo->setAlignment(Qt::AlignCenter);

    this->loginUsername = new QLabel(QObject::tr("Username:"), this);
    this->editUsername = new QLineEdit(this);
    this->localIPAddress = new QLabel(QObject::tr("Local IP:"), this);
    this->localIPAddressBox = new QComboBox(this);
    this->localPort = new QLabel(QObject::tr("Local Port:"), this);
    this->localPortBox = new QSpinBox(this);
    this->registerIpLabel = new QLabel(QObject::tr("Server IP:"), this);
    this->registerIpEdit = new QLineEdit(this);
    this->registerPortLabel = new QLabel(QObject::tr("Server Port"), this);
    this->registerPortBox = new QSpinBox(this);
    this->registerIpLabel->setBuddy(this->registerIpEdit);
    this->registerPortLabel->setBuddy(this->registerPortBox);
    this->loginUsername->setBuddy(this->editUsername);
    this->localIPAddress->setBuddy(this->localIPAddressBox);
    this->localPort->setBuddy(this->localPortBox);

    this->networkInterfacesCombo();
    this->localPortBox->setRange(1024, 65535);
    this->localPortBox->setValue(9001);
    this->registerPortBox->setRange(1024, 65535);
    this->registerPortBox->setValue(10001);
    this->editUsername->setText("unknown");
    this->registerIpEdit->setText("192.168.1.106");

    // Button Settings
    this->doneButton = new QPushButton(QObject::tr("Register"), this);
    this->cancelButton = new QPushButton(QObject::tr("Cancel"), this);
    this->doneButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->cancelButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->initLayout();
    this->resize(QSize(300, 220));
    this->setMinimumSize(QSize(250, 180));
    this->setWindowTitle(QObject::tr("Client Register"));

    connect(doneButton, SIGNAL(clicked()), this, SLOT(on_doneButton_clicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(on_cancelButton_clicked()));
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::networkInterfacesCombo()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach( QHostAddress address, list )
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            this->localIPAddressBox->addItem(address.toString());
    }
}

void LoginDialog::initLayout()
{
    // Layout Manager
    QHBoxLayout *userLayout = new QHBoxLayout;
    userLayout->addWidget(this->loginUsername);
    userLayout->addWidget(this->editUsername);
    userLayout->setStretch(0, 1);
    userLayout->setStretch(1, 3);

    QHBoxLayout *ipLayout = new QHBoxLayout;
    ipLayout->addWidget(this->localIPAddress);
    ipLayout->addWidget(this->localIPAddressBox);
    ipLayout->setStretch(0, 1);
    ipLayout->setStretch(1, 3);

    QHBoxLayout *portLayout = new QHBoxLayout;
    portLayout->addWidget(this->localPort);
    portLayout->addWidget(this->localPortBox);
    portLayout->setStretch(0, 1);
    portLayout->setStretch(1, 3);

    QHBoxLayout *ripLayout = new QHBoxLayout;
    ripLayout->addWidget(this->registerIpLabel);
    ripLayout->addWidget(this->registerIpEdit);
    ripLayout->setStretch(0, 1);
    ripLayout->setStretch(1, 3);

    QHBoxLayout *rportLayout = new QHBoxLayout;
    rportLayout->addWidget(this->registerPortLabel);
    rportLayout->addWidget(this->registerPortBox);
    rportLayout->setStretch(0, 1);
    rportLayout->setStretch(1, 3);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(this->doneButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(this->cancelButton);
    buttonLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(this->loginLogo);
    mainLayout->addLayout(userLayout);
    mainLayout->addLayout(ipLayout);
    mainLayout->addLayout(portLayout);
    mainLayout->addLayout(ripLayout);
    mainLayout->addLayout(rportLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    mainLayout->setStretch(0, 2);
    mainLayout->setStretch(1, 1);
    mainLayout->setStretch(2, 1);
    mainLayout->setStretch(3, 1);
    mainLayout->setStretch(4, 1);
    mainLayout->setStretch(5, 1);
    mainLayout->setStretch(6, 1);

    this->setLayout(mainLayout);
}


QString LoginDialog::getUsername()
{
    QString username = this->editUsername->text();
    if(username.isEmpty())
        username = "unknown";

    return username;
}

QString LoginDialog::getLocalServerIP()
{
    QString ipAddress = this->localIPAddressBox->currentText();
    if(ipAddress.isEmpty())
        ipAddress = "localhost";

    return ipAddress;
}

int LoginDialog::getLocalServerPort()
{
    return this->localPortBox->value();
}

QString LoginDialog::getRegisterServerIP()
{
    QString serverIp = this->registerIpEdit->text();
    if(serverIp.isEmpty())
        serverIp = "192.168.1.106";

    return serverIp;
}

int LoginDialog::getRegisterServerPort()
{
    return this->registerPortBox->value();
}

void LoginDialog::on_doneButton_clicked()
{
    this->accept();
}

void LoginDialog::on_cancelButton_clicked()
{
    this->reject();
}
