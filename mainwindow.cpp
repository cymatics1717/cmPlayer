#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QListWidget>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),ui(new Ui::MainWindow),scene(new QGraphicsScene(this))
  ,tip(new QLabel(this)),context_m(new QMenu(this))
{
    ui->setupUi(this);
    setCentralWidget(ui->center);
    ui->stackedWidget->setFixedSize(640,480);

    context_m->addAction("&add",this,SLOT(addDevice()));
    context_m->addAction("&toggle",this,SLOT(togglePage()));
    context_m->addAction("&quit",qApp,SLOT(quit()));

    statusBar()->addPermanentWidget(tip);
    QMetaObject::invokeMethod(ui->player,"init");
    startTimer(0);
//    qDebug()<<centralWidget()->size();

    ui->view->setScene(scene);
    ui->view->setBackgroundBrush(QBrush(QColor(100,100,100,200)));

    scene->addPixmap(QPixmap("./hk2017-09-15_17-48-49_260.jpg").scaledToWidth(250));
    ui->player->addDevice({"","172.16.1.250",8000,"admin","1234abcd"});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        context_m->exec(QCursor::pos());
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    tip->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
}

void MainWindow::addDevice()
{
    deviceConfig dev;
    if(dev.exec()==QDialog::Accepted){
        userInfo user = {dev.aliasName(),dev.ip(),dev.port(),dev.username(),dev.password()};
        togglePage();
        ui->player->addDevice(user);
    }
}

void MainWindow::togglePage()
{
    static int cnt = 0;
    ui->stackedWidget->setCurrentIndex(cnt%2);

    QString msg;
    if(cnt%2==0){
        msg = QString("%1:real time preview").arg(cnt);
    } else {
        msg = QString("%1:after decoding").arg(cnt);
    }
    statusBar()->showMessage(msg);
    cnt++;
}

void MainWindow::on_player_incoming(const QImage &image)
{
    ui->player2->draw(image);
}

void MainWindow::on_player_incoming2(const QImage &image)
{
    scene->addPixmap(QPixmap::fromImage(image).scaledToWidth(250));
}
