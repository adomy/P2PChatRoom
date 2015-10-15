#include "logindialog.h"
#include "mainwindow.h"
#include "chatdialog.h"
#include <QApplication>

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void clearLogFile();

int main(int argc, char *argv[])
{
    clearLogFile();
    qInstallMessageHandler(outputMessage);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

void clearLogFile()
{
    QFile file("chatLog.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    file.resize(0);
    file.flush();
    file.close();
}

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("(%1:%2)").arg(QString(context.file)).arg(context.line);
    QString message = QString("%1 %2\r\n %3").arg(text).arg(context_info).arg(msg);

    mutex.lock();

    QFile file("chatLog.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}
