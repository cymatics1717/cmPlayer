#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->stackedWidget);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    context_m = new QMenu(this);
    context_m->addAction("add device",this,SLOT(addDevice()));
    ui->toolBar->addAction("add device",this,SLOT(addDevice()));
    ui->toolBar->addAction("toggle preview",this,SLOT(togglePage()));

    QMetaObject::invokeMethod(ui->player,"init");
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

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<event->text();
    if(event->key() == Qt::Key_Return){
        addDevice();
    }
}

void MainWindow::addDevice()
{
    deviceConfig dev;
    if(dev.exec()==QDialog::Accepted){
        userInfo user = {dev.aliasName(),dev.ip(),dev.port(),dev.username(),dev.password()};
        ui->player->addDevice(user);
    }
}

void MainWindow::togglePage()
{
    static int cnt = 0;
    cnt++;
    ui->stackedWidget->setCurrentIndex(cnt%2);
}

void MainWindow::on_player_incoming(const QImage &image)
{
    ui->player2->draw(image);
}
