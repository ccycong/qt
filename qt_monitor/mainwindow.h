#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/qtcpsocket.h>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/qabstractsocket.h>
#include <QFile>
#include <QString>
#include <QObject>
#include <QHostAddress>
#include <stdlib.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void sendString(QString);
public slots:
    void test();
    void Tran();
    void runMessage();
    void readMessage();
    void writeMessage();

private:
    Ui::MainWindow *ui;
    QTcpSocket *client;
    QString info;
    QString message;
    //QString na;
   // QString pcCMD;
    QFile file;
    QString filename;
    qint32 filesize;
    qint32 recvsize;
    bool isStart;
    QThread *qm;
    QThread *qf;
};
#endif // MAINWINDOW_H
