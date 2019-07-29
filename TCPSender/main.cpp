#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    
    return a.exec();
}
