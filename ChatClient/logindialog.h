#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "header.h"
#include <QDialog>


class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    QString getUsername();
    QString getLocalServerIP();
    int getLocalServerPort();
    QString getRegisterServerIP();
    int getRegisterServerPort();

signals:


public slots:
    void on_doneButton_clicked();
    void on_cancelButton_clicked();

private:
    // private functions
    void networkInterfacesCombo();
    void initLayout();

    // login information
    QLabel *loginLogo;
    QLabel *loginUsername;
    QLineEdit *editUsername;
    QLabel *localIPAddress;
    QComboBox *localIPAddressBox;
    QLabel *localPort;
    QSpinBox *localPortBox;
    QLabel *registerIpLabel;
    QLineEdit *registerIpEdit;
    QLabel *registerPortLabel;
    QSpinBox *registerPortBox;
    QPushButton *doneButton;
    QPushButton *cancelButton;

};

#endif // LOGINDIALOG_H
