#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QHttpPart>
#include "imageitem.h"
#include <QtWidgets>

#include "common.h"
#define M 300

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),ui(new Ui::MainWindow),scene(new QGraphicsScene(this))
  ,tip(new QLabel(this)),context_m(new QMenu(this))
{
    ui->setupUi(this);
    setCentralWidget(ui->center);
    connect(ui->player,SIGNAL(incoming(QImage)),ui->player2,SLOT(draw(QImage)));
    context_m->addAction("&add",this,SLOT(addDevice()));
    context_m->addAction("&toggle",this,SLOT(togglePage()));
    context_m->addAction("&quit",qApp,SLOT(quit()));

    statusBar()->addPermanentWidget(tip);
    startTimer(0);
    timerID = startTimer(30);

    ui->view->setScene(scene);
    ui->view->setBackgroundBrush(QBrush(QColor("#2E2F30")));

    installEventFilter(this);

//    FaceSetList();
    faceset_id="j5BLlJ348wh2g61O83TM1dg7834mQ4E7";
//    FaceSetRemove();
    DetectCarPlate();
    FaceList();

    QTimer::singleShot(100,this,SLOT(init()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    //    QSet<QEvent::Type> buff={QEvent::Timer,QEvent::UpdateRequest,QEvent::Paint,QEvent::LayoutRequest,QEvent::HoverMove};
    //    if(!buff.contains(e->type())){
    //        qDebug()<<obj<< "---------"<<e->type();
    //    }
    if(e->type() == QEvent::KeyPress){
        QKeyEvent *event = static_cast<QKeyEvent *>(e);
        if(event){
            //            qDebug() << event->text();
            //            return true;
            switch (event->key()) {
            case Qt::Key_Q:
                qApp->quit();
                break;
            case Qt::Key_Return:
            {
                static int cnt =0;
                if(++cnt%2==1){
                    showFullScreen();
                } else {
                    showNormal();
                }
            }
                break;
            case Qt::Key_Space:
            {
                togglePage();
            }
                break;
            default:
                break;
            }
        }
    } else if(e->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *event = static_cast<QMouseEvent *>(e);
        if(event&&event->button()==Qt::RightButton){
            context_m->exec(QCursor::pos());
        }
    } else if(e->type() == QEvent::Timer){
        QTimerEvent *event = static_cast<QTimerEvent *>(e);
        if(event->timerId()==timerID){

        } else {
            tip->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
        }
    } else if(e->type() == QEvent::HoverMove){
        QHoverEvent *event = static_cast<QHoverEvent *>(e);
        if(event){
            imageItem *item = static_cast<imageItem *>(ui->view->itemAt(ui->view->mapFrom(this,event->pos())));
            if(item){
                item->setToolTip("here,this is pictwerthis is pictwerthis\n"
                                 " is picttwerthis is pictwerth"
                                 "icttwerthis is pictwerthis is pictwert");
            }
        }
    }

    return QObject::eventFilter(obj,e);
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

void MainWindow::FaceSetRegister()
{
    QUrl uu = QString("http://%1%2").arg(CM_HOST).arg(CM_FACESET_REGISTER);
    dump.insert(uu,std::bind(&MainWindow::onFaceSetRegisterReply, this,std::placeholders::_1));
    post(uu);
//    get(uu);
}

void MainWindow::onFaceSetRegisterReply(const QJsonObject &obj){
    qDebug() << obj;
    if(obj.contains("faceset_id")){
        faceset_id = obj.value("faceset_id").toString();
        qDebug() << faceset_id;
        FaceSetList();
    }
}

void MainWindow::FaceSetList()
{
    QUrl uu = QString("http://%1%2").arg(CM_HOST).arg(CM_FACESET_LIST);
    dump.insert(uu,std::bind(&MainWindow::onFaceSetListReply, this,std::placeholders::_1));
    post(uu);
}

void MainWindow::onFaceSetListReply(const QJsonObject &obj)
{
    qDebug() << obj;
//    FaceAdd();
}

void MainWindow::FaceSetRemove()
{
    QList<QHttpPart> lst;
    ///////////////////////////////////////////////
    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"faceset_id\""));
    textPart.setBody(faceset_id.toUtf8());
    lst.append(textPart);
    ///////////////////////////////////////////////
    QUrl uu = QString("http://%1%2").arg(CM_HOST).arg(CM_FACESET_REMOVE);
    dump.insert(uu,std::bind(&MainWindow::onFaceSetRemoveReply, this,std::placeholders::_1));
    post(uu,lst);
//    get(uu);
}

void MainWindow::onFaceSetRemoveReply(const QJsonObject &obj){
    qDebug() << obj;
}

void MainWindow::FaceAdd()
{
    QList<QHttpPart> lst;
    ///////////////////////////////////////////////
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"faceset_id\""));
        textPart.setBody(faceset_id.toUtf8());
        lst.append(textPart);
    }
    ///////////////////////////////////////////////
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
        textPart.setBody(QUrl::toPercentEncoding(QString("Test from Qt:%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HHmmss.zzz"))));
        lst.append(textPart);
    }
    ///////////////////////////////////////////////
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    ui->player->cap.save(&buffer, "JPEG");

    QHttpPart imagePart;
    QString other = QString("filename=\"FromQt%1.jpg\";")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
    QString fakeit =QString("form-data;%1name=\"imagefile\"").arg(other);
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant(fakeit.toStdString().c_str()));
    imagePart.setBody(arr);
    lst.append(imagePart);

    ///////////////////////////////////////////////
    QUrl uu = QString("http://%1%2").arg(CM_HOST).arg(CM_FACE_ADD);
    dump.insert(uu,std::bind(&MainWindow::onFaceAddReply, this,std::placeholders::_1));
    post(uu,lst);
}

void MainWindow::onFaceAddReply(const QJsonObject &obj)
{
    qDebug() << obj;
}

void MainWindow::FaceRecog()
{
    QList<QHttpPart> lst;
    ///////////////////////////////////////////////
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"faceset_id\""));
        textPart.setBody(faceset_id.toUtf8());
        lst.append(textPart);
    }
    ///////////////////////////////////////////////
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    QImage img("/home/wayne/1231.jpg");
    img.save(&buffer, "JPEG");
//    ui->player->cap.save(&buffer, "JPEG");

    QHttpPart imagePart;
    QString other = QString("filename=\"FromQt%1.jpg\";").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
    QString fakeit =QString("form-data;%1name=\"imagefile\"").arg(other);
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant(fakeit.toStdString().c_str()));
    imagePart.setBody(arr);
    lst.append(imagePart);

    ///////////////////////////////////////////////
    QUrl uu = QString("http://%1%2").arg(CM_HOST).arg(CM_FACE_RECOG);
    dump.insert(uu,std::bind(&MainWindow::onFaceRecogReply, this,std::placeholders::_1));
    post(uu,lst);
}

void MainWindow::onFaceRecogReply(const QJsonObject &obj)
{
    qDebug() << obj;
}

void MainWindow::Facesetname()
{
    QList<QHttpPart> lst;
    ///////////////////////////////////////////////
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
        textPart.setBody(QUrl::toPercentEncoding("my name is LiLei"));
        lst.append(textPart);
    }
    ///////////////////////////////////////////////
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"face_id\""));
        textPart.setBody(faceset_id.toUtf8());
        lst.append(textPart);
    }
    ///////////////////////////////////////////////
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"faceset_id\""));
        textPart.setBody(faceset_id.toUtf8());
        lst.append(textPart);
    }

    ///////////////////////////////////////////////
    QUrl uu = QString("http://%1%2").arg(CM_HOST).arg(CM_FACE_SET_NAME);
    dump.insert(uu,std::bind(&MainWindow::onFacesetnameReply, this,std::placeholders::_1));
    post(uu,lst);
}

void MainWindow::onFacesetnameReply(const QJsonObject &obj)
{
    qDebug() << obj;
}

void MainWindow::FaceList()
{
    QList<QHttpPart> lst;
    ///////////////////////////////////////////////
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"faceset_id\""));
        textPart.setBody(faceset_id.toUtf8());
        lst.append(textPart);
    }
    ///////////////////////////////////////////////
    QUrl uu = QString("http://%1%2").arg(CM_HOST).arg(CM_FACE_LIST);
    dump.insert(uu,std::bind(&MainWindow::onFaceListReply, this,std::placeholders::_1));
    post(uu,lst);
}

void MainWindow::onFaceListReply(const QJsonObject &obj)
{
    qDebug() << obj;
}

void MainWindow::FaceRemove()
{
    QList<QHttpPart> lst;
    ///////////////////////////////////////////////
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"faceset_id\""));
        textPart.setBody(faceset_id.toUtf8());
        lst.append(textPart);
    }
    ///////////////////////////////////////////////
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"face_id\""));
        textPart.setBody(faceset_id.toUtf8());
        lst.append(textPart);
    }
    ///////////////////////////////////////////////
    QUrl uu = QString("http://%1%2").arg(CM_HOST).arg(CM_FACE_REMOVE);
    dump.insert(uu,std::bind(&MainWindow::onFaceRemoveReply, this,std::placeholders::_1));
    post(uu,lst);
}

void MainWindow::onFaceRemoveReply(const QJsonObject &obj)
{
    qDebug() << obj;
}

void MainWindow::DetectCarPlate()
{
    QList<QHttpPart> lst;
    ///////////////////////////////////////////////
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    QImage img("/home/wayne/123.jpg");
    img.save(&buffer, "JPEG");

    QHttpPart imagePart;
    QString other = QString("filename=\"FromQt%1.jpg\";").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
    QString fakeit =QString("form-data;%1name=\"imagefile\"").arg(other);
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant(fakeit.toStdString().c_str()));
    imagePart.setBody(arr);

    lst.append(imagePart);

    ///////////////////////////////////////////////
    QUrl uu = QString("http://%1%2").arg(CM_HOST).arg(CM_DETECT_CAR_PLATE);
    dump.insert(uu,std::bind(&MainWindow::onDetectCarPlateReply, this,std::placeholders::_1));
    post(uu,lst);
}

void MainWindow::onDetectCarPlateReply(const QJsonObject &obj)
{
    qDebug() << obj;
}

void MainWindow::get(const QUrl &u)
{
    QUrlQuery param;
    param.addQueryItem("app_key",app_key);
    param.addQueryItem("app_secret",app_secret);

    QUrl uu = u;
    uu.setQuery(param);
    QNetworkRequest req(uu);
    QNetworkReply *reply = manager.get(req);
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(onError(QNetworkReply::NetworkError)));
    connect(reply,SIGNAL(finished()),SLOT(onFinished()));

    qDebug() << req.url();
}

void MainWindow::post(const QUrl &u, const QList<QHttpPart> &parts)
{
    QUrlQuery param;
    param.addQueryItem("app_key",app_key);
    param.addQueryItem("app_secret",app_secret);

    QUrl uu = u;
    uu.setQuery(param);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    for(auto part: parts) multiPart->append(part);
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"app_key\""));
        textPart.setBody(QString(app_key).toUtf8());
        multiPart->append(textPart);
    }
    {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"app_secret\""));
        textPart.setBody(QString(app_secret).toUtf8());
        multiPart->append(textPart);
    }

    QNetworkRequest req(uu);
    QNetworkReply *reply = manager.post(req, multiPart);
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(onError(QNetworkReply::NetworkError)));
    connect(reply,SIGNAL(finished()),SLOT(onFinished()));

    multiPart->setParent(reply);
    qDebug() <<multiPart<< req.url();
}

void MainWindow::addPixmap(QJsonObject obj)
{
    imageItem *item = new imageItem(QPixmap("/home/wayne/puffin.png"));
    QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem("~/puffin.png",item);
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

void MainWindow::onFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    if(reply){
        QByteArray ans = reply->readAll();

        QJsonDocument doc = QJsonDocument::fromJson(ans);
        if(doc.isObject()){
            if(dump.contains(reply->url().url(QUrl::RemoveQuery))){
                dump.value(reply->url().url(QUrl::RemoveQuery))(doc.object());
            }
        } else {
            qDebug() << "invalid json:" << ans;
        }
        reply->deleteLater();
    }
}

void MainWindow::onError(QNetworkReply::NetworkError err)
{
    qDebug() <<err;
}

void MainWindow::init()
{
    statusBar()->showMessage("testing webcam connection...");
    QTcpSocket *tcp = new QTcpSocket(this);
    connect(tcp,SIGNAL(connected()),SLOT(onConnected()));
    connect(tcp,SIGNAL(error(QAbstractSocket::SocketError))
            ,SLOT(onError(QAbstractSocket::SocketError)));
    connect(tcp,SIGNAL(stateChanged(QAbstractSocket::SocketState))
            ,SLOT(onStateChanged(QAbstractSocket::SocketState)));
    tcp->connectToHost(WEBCAM_HOST,WEBCAM_PORT);
}

void MainWindow::onConnected()
{
    statusBar()->showMessage("webcam is online...");
    QMetaObject::invokeMethod(ui->player,"init");
    ui->player->addDevice({"",WEBCAM_HOST,WEBCAM_PORT,WEBCAM_USER,WEBCAM_CODE});
}

void MainWindow::onError(QAbstractSocket::SocketError err)
{
    QTcpSocket *tcp = static_cast<QTcpSocket *>(sender());
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    statusBar()->showMessage(metaEnum.valueToKey(err));
    qDebug()<< QString("webcam host %1:%2 connection ERROR: %3")
               .arg(WEBCAM_HOST).arg(WEBCAM_PORT).arg(qPrintable(tcp->errorString()));
}

void MainWindow::onStateChanged(QAbstractSocket::SocketState st)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    statusBar()->showMessage(QString("connecting: %1").arg(metaEnum.valueToKey(st)));
}

