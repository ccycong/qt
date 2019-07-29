#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client =new QTcpSocket();

    //tcp  messsage   socket
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(runMessage()));

    // write  message
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(writeMessage()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(writeMessage()));
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(writeMessage()));
    connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(writeMessage()));


    // printf
    connect(this, SIGNAL(sendString(QString)), ui->textEdit, SLOT(append(QString)));
    connect(client,SIGNAL(readyRead()),this,SLOT(readMessage()));
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::runMessage(){
    //client->connectToHost("192.168.46.230",8848);
    client->connectToHost(ui->lineEdit->text(),ui->lineEdit_2->text().toInt());
    //client->write("ccy")
}

void MainWindow::Tran(){
    //QString ip = ui->lineEdit->text();
    //quint8 port = ui->lineEdit_2->text().toInt();
    //client->connectToHost(ui->lineEdit->text(),ui->lineEdit_2->text().toInt());


    //client1->connectToHost("localhost",8848);

}

void MainWindow::test(){
//    const char na[255]="ccy";
//    char pcCMD[255];
//    sprintf(pcCMD,"mkdir %s",na);
//    system(pcCMD);

//    QString data =ui->lineEdit_3->text();
//    client->write(data.toStdString().c_str());


}
void MainWindow::writeMessage(){
    QPushButton *p = (QPushButton*)sender();
    QString name = p->text();
    if(name == "Preview") {
       client->write(name.toStdString().c_str());
    }
    if(name == "Capture") {
       client->write(name.toStdString().c_str());
    }
    if(name == "StopCap") {
        client->write(name.toStdString().c_str());
    }
    if(name == "Unpre") {
        client->write(name.toStdString().c_str());
    }
//    QString info =ui->lineEdit_3->text();
//    client->write(info.toStdString().c_str());
}
void MainWindow::readMessage(){
    QString message =client->readAll();
    ui->textEdit->setText(message);

}
