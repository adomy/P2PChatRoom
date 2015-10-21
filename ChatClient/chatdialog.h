#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include "header.h"
#include "chatconnection.h"
#include <QDialog>

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = 0, QString username = "localhost", QString peername = "unknown", QString peeraddr = "null", ChatConnection *connection = NULL);
    ~ChatDialog();

    void readAndDisplayHistory(QList<QString>);
public slots:
    void handleReceivedMssage(QString from, QString message);
    void handleToSendMessage();
    void handleConnectionError(QAbstractSocket::SocketError);
    void handleRejected();

private:
    // private functions
    void initLayout();

    /*Qt Widgets*/
    //QLabel *iconLabel;
    QLabel *peerName;
    QLabel *peerAddress;
    QTextEdit *logChat;
    QLineEdit *msgSend;
    QPushButton *sendButton;

    // Connection part
    QString userName;
    ChatConnection *chatSession;
};

#endif // CHATDIALOG_H
