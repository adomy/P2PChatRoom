#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "header.h"
#include "chatconnection.h"
#include "registerconnection.h"
#include "chatserver.h"
#include "logindialog.h"
#include "chatdialog.h"
#include "peerinfo.h"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void handleRegisterAction();
    void handleNewPeerList(QString &);
    void handleActiveConnection();
    void handleCheckDialog();
    void handlePassiveConnection(ChatConnection *);
    void handleChatConnectionError(ChatConnection *, QAbstractSocket::SocketError);
    void handleRegisterError(QAbstractSocket::SocketError);
    void handleNewChatDialog(ChatConnection *);
    void handlePassiveMsgRecv(QString, QString);
    void handlePassiveNewChat(ChatConnection *);
    void handleDialogExit(ChatConnection *);

private:
    // private functions
    void initLayout();
    void initNetwork();
    void updateListWidget();

    // connections
    void addChatConnection(ChatConnection *);
    void removeChatConnection(ChatConnection *);
    ChatConnection* findChatConnection(QString name, QString ip);

    // gui widgets
    QLabel *myName;
    QLabel *myAddress;
    //QLabel *myIcon;
    QLabel *infoLabel;
    QListWidget *peerListWidget;
    QPushButton *chatButton;
    QPushButton *registerButton;
    QPushButton *checkButton;
    QLabel *remoteServerLabel;

    // Variables
    QString userName; // Local Server UserName
    QHostAddress localServerIP;
    int localServerPort;
    QHostAddress remoteServerIP;
    int remoteServerPort;

    // connections
    QMap<QString, QList<QString>> tempMsgs;  //记录被动连接的接收信息
    QMap<QString, ChatDialog *> chatMap;  //记录已打开的ChatDialog
    QList<PeerInfo *> peerList; // 记录活跃的远端主机
    QList<ChatConnection *> connList; // 用于管理所有聊天连接
    ChatServer *server; // 本地聊天连接监听服务器
    RegisterConnection *registConnection; // 与中心服务器相连接
};

#endif // MAINWINDOW_H
