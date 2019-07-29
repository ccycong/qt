#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //qc

    qc =new  qthreadcamera(this);
    //qf =new  qthreadMessgge(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(runcamera()));

    //message
    server =new QTcpServer();
    server->listen(QHostAddress::Any, 8848);
   // emit(sendString("capture"));
    connect(server, SIGNAL(newConnection()), this, SLOT(newMessageClient()));
    connect(this, SIGNAL(sendString(QString)), ui->textEdit, SLOT(append(QString)));
   //connect(qf, SIGNAL(sendMessage(QString)), ui->textEdit, SLOT(append(QString)));
    //thread camera tongxin
    connect(this,SIGNAL(setencap()),qc,SLOT(setencapfunc()),Qt::QueuedConnection);
    connect(this,SIGNAL(setenpre()),qc,SLOT(setenprefunc()),Qt::QueuedConnection);
    connect(this,SIGNAL(setuncap()),qc,SLOT(setuncapfunc()),Qt::QueuedConnection);
    connect(this,SIGNAL(setunpre()),qc,SLOT(setunprefunc()),Qt::QueuedConnection);
    //connect的第五个参数Qt::QueuedConnection表示槽函数由接受信号的线程所执行，如果不加表示槽函数由发出信号的次线程执行
    //file

    ///connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(test()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
//void MainWindow::test(){
//    emit(setencap());
//}

void MainWindow::runcamera(){
     qc->start();
}
void MainWindow::newMessageClient(){
    //emit(sendString("capture"));
    client = server->nextPendingConnection();
    connect(client, SIGNAL(readyRead()), this, SLOT(readMessage()));
    client->write("socket is created");

}

void MainWindow::readMessage(){
    emit(sendString("capture"));
    QString data = client->readAll();
    if (data=="Capture"){
         client->write("can read cap");
         emit(setencap());
}
if (data=="Preview"){
    client->write("can read pre");
    emit(setenpre());
}
    if (data=="StopCap"){
    //将接收到的数据存放到变量中
    emit(setuncap());
    client->write("can read stopcap");
}
    if (data=="Unpre"){
    emit(setunpre());
    client->write("can read Unpre");
}
}
void MainWindow::writeMessage(){

}


