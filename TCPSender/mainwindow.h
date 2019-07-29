#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void send();  //连接服务器
    void startTransfer();  //发送文件大小等信息
    void updateClientProgress(qint64); //发送数据，更新进度条
    void displayError(QAbstractSocket::SocketError); //显示错误
    void openFile();  //打开文件
    
private slots:
    void on_openButton_clicked();

    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpClient;
        QFile *localFile;  //要发送的文件
        qint64 totalBytes;  //数据总大小
        qint64 bytesWritten;  //已经发送数据大小
        qint64 bytesToWrite;   //剩余数据大小
        qint64 loadSize;   //每次发送数据的大小
        QString fileName;  //保存文件路径
    QByteArray outBlock;  //数据缓冲区，即存放每次要发送的数据
};

#endif // MAINWINDOW_H
