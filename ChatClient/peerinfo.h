#ifndef PEERINFO_H
#define PEERINFO_H

#include "header.h"
#include <QObject>

class PeerInfo : public QObject
{
    Q_OBJECT
public:
    explicit PeerInfo(QObject *parent = 0, QString peername = "unknown", QHostAddress ip = QHostAddress("127.0.0.1"), int port = 0);
    ~PeerInfo();

    QString getPeerName();
    QString getPeerIP();
    int getPeerPort();
    void setPeerInfo(QString, QHostAddress, int);
    void clear();

private:
    QString peerName;
    QHostAddress peerIP;
    int peerPort;
};

#endif // PEERINFO_H
