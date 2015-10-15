#ifndef WINDOW_H
#define WINDOW_H

#include "header.h"
#include "peerinfo.h"
#include "registerconnection.h"
#include "registerserver.h"
#include <QMainWindow>

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = 0);
    ~Window();

public slots:
    void startNetwork();
    void stopNetwork();
    void handleNewConnection(RegisterConnection *connection);
    void handleRegisterError(RegisterConnection *, QAbstractSocket::SocketError);
    void handleNewRegisterMsg(RegisterConnection *, QString message);
    void handlePeerTimeout(PeerInfo *);

private:
    // 私有函数及方法（主要包括界面初始化、网络操作工具函数等）
    void initLayout();
    void networkInterfacesCombo();
    void updateRegisterListWidget();
    void appendLogInfo(QString);

    QString genPeerListStr();
    PeerInfo* findPeer(QString, QHostAddress, int);
    RegisterConnection* findConnection(PeerInfo *);

    void addRegisterConnection(RegisterConnection *);
    void removeRegisterConnection(RegisterConnection *);
    void addPeerInfoIntoList(PeerInfo *);
    void removePeerInfoFromList(PeerInfo *);
    void broadcastPeerChange();

    // 私有成员变量（主要与网络相关）
    QMutex mutex1, mutex2;
    RegisterServer *server;
    QList<RegisterConnection *> registerConns;
    QList<PeerInfo *> peerList;
    QHostAddress serverIP;
    int serverPort;

    // 私有成员变量（主要与界面相关）
    QLabel *listLabel;
    QListWidget *registerListWidget;
    QLabel *logLabel;
    QTextEdit *logRegister;
    QLabel *ipLabel;
    QComboBox *ipBox;
    QLabel *portLabel;
    QSpinBox *portBox;
    QPushButton *startButton;
    QPushButton *stopButton;

};

#endif // WINDOW_H
