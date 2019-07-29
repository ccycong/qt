#ifndef QTHREADCAMERA_H
#define QTHREADCAMERA_H
#include <QThread>
#include <QTime>
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
}
class qthreadcamera :public QThread
{
    Q_OBJECT
public:
    explicit  qthreadcamera(QObject *parent = 0);
    void run();
//signals:
   //void setencap();
public slots:
    void setencapfunc();
    void setenprefunc();
    void setuncapfunc();
    void setunprefunc();
private:
    QString strName;
};
#endif // QTHREADCAMERA_H
//void MainWindow::closeMessageThread(){

//      if (qm->isRunning()){
//            qm->quit();
//        }
//      if(!(qm->isRunning())){

//          ui->textEdit->append("MessageThread is quit");
//      }
//}
