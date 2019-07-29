#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QtNetwork/qtcpsocket.h>
#include <QtNetwork/qabstractsocket.h>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QFile>
#include <QString>
#include <QObject>
#include <stdlib.h>
#include <QHostAddress>
#include <QFileDialog>
#include <QFileInfo>
#include <qthreadcamera.h>
#include <qthreadfile.h>
#include <qthreadmessgge.h>
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
    void setencap();
    void setenpre();
    void setuncap();
    void setunpre();

public slots:
     void runcamera();
     void readMessage();
     void writeMessage();
     void newMessageClient();
    //void test();

private:
       Ui::MainWindow *ui;
       QTcpServer *server;
       QTcpSocket *client;

//       QFile file;//文件对象
//       QString fileName;//文件名
//       qint64 fileSize;//文件大小
//       qint64 sendSize;//已经发送文件的大小

       QTimer timer;
       //QThread *qm;
     //  QThread *qf;
       QThread *qc;


};

#endif // MAINWINDOW_H
