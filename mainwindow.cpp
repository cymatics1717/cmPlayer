#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QHttpPart>
#include "imageitem.h"
#include <QtWidgets>
#define M 300

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),ui(new Ui::MainWindow),scene(new QGraphicsScene(this))
  ,tip(new QLabel(this)),context_m(new QMenu(this))
{
    ui->setupUi(this);
    setCentralWidget(ui->center);
    connect(ui->player,SIGNAL(incoming(QImage)),ui->player2,SLOT(draw(QImage)));
    connect(&manager,SIGNAL(finished(QNetworkReply*)),SLOT(onFinished(QNetworkReply*)));
    context_m->addAction("&add",this,SLOT(addDevice()));
    context_m->addAction("&toggle",this,SLOT(togglePage()));
    context_m->addAction("&quit",qApp,SLOT(quit()));

    statusBar()->addPermanentWidget(tip);
    QMetaObject::invokeMethod(ui->player,"init");
    startTimer(0);
    timerID = startTimer(30);
//    qDebug()<<centralWidget()->size();

    ui->view->setScene(scene);
    ui->view->setBackgroundBrush(QBrush(QColor("#2E2F30")));

    ui->player->addDevice({"","172.16.1.250",8000,"admin","1234abcd"});

    addPixmap(QJsonObject());
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
    if(event->timerId() == timerID){
        post();
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
    static int cnt = 1;
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

void MainWindow::post()
{
    QString dat = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
///////////////////////////////////////////////
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"Imagename\""));
    textPart.setBody(QString("Test from Qt:%1").arg(dat).toUtf8());
///////////////////////////////////////////////
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    ui->player->cap.save(&buffer, "JPEG");;

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"imagefile\""));
    imagePart.setBody(arr);
///////////////////////////////////////////////
    multiPart->append(imagePart);
    multiPart->append(textPart);

    QUrl url("http://www.cymatics.cc");
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.post(request, multiPart);
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(onError(QNetworkReply::NetworkError)));
    multiPart->setParent(reply);
    qDebug() << dat<< request.url();
}

void MainWindow::addPixmap(QJsonObject obj)
{
    imageItem *item = new imageItem(QPixmap("./hk2017-09-18_16-45-50_439.jpg"));
    QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem("hk2017-09-18_16-45-50_439",item);
    QFont font;
    font.setPointSize(20);
    text->setFont(font);
    text->setBrush(QBrush(Qt::red));
    text->setPos(item->pos()+QPoint(0,item->boundingRect().height()/2));
    scene->addItem(item);
    //    scene->addItem(buttonParent);
    QPropertyAnimation *ani = new QPropertyAnimation(item,"pos");
    //    connect(ani,SIGNAL(finished()),this,SLOT(fadingAway()));
    qDebug() << scene->sceneRect()   ;
    int m = M;
    double r =2.3 ;
    ani->setDuration(3000);
    ani->setStartValue(QPoint(r*m,-m));
    ani->setEndValue(QPoint(-r*m,-m));
    ani->setEasingCurve(QEasingCurve::OutBounce);
    ani->start(QAbstractAnimation::DeleteWhenStopped);
    ui->view->ensureVisible(item);
}

void MainWindow::onFinished(QNetworkReply *reply)
{
    qDebug() <<reply->request().rawHeaderList()<<reply->readAll();
}

void MainWindow::onError(QNetworkReply::NetworkError err)
{
    qDebug() <<err;
}
