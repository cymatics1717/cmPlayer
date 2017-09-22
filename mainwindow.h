#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QHttpPart>
#include <QJsonObject>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include "deviceconfig.h"
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <functional>
//#include <unordered_map>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool eventFilter (QObject *obj, QEvent *event);

public slots:
    void addDevice();
    void togglePage();

    void get(const QUrl &u);
    void post(const QUrl &u, const QList<QHttpPart> &parts = QList<QHttpPart>());
    void addPixmap(QJsonObject obj);
    void onFinished();
    void fadingAway();
    void onError(QNetworkReply::NetworkError);

private slots:
    void init();
    void onConnected();
    void onError(QAbstractSocket::SocketError);
    void onStateChanged(QAbstractSocket::SocketState);

    void FaceSetRegister();
    void onFaceSetRegisterReply(const QJsonObject &obj);
    void FaceSetList();
    void onFaceSetListReply(const QJsonObject &obj);
    void FaceSetRemove();
    void onFaceSetRemoveReply(const QJsonObject &obj);
    void FaceAdd();
    void onFaceAddReply(const QJsonObject &obj);
    void FaceRecog();
    void onFaceRecogReply(const QJsonObject &obj);
    void Facesetname();
    void onFacesetnameReply(const QJsonObject &obj);
    void FaceList();
    void onFaceListReply(const QJsonObject &obj);
    void FaceRemove();
    void onFaceRemoveReply(const QJsonObject &obj);
    void DetectCarPlate();
    void onDetectCarPlateReply(const QJsonObject &obj);

private:
    Ui::MainWindow *ui;
    QMenu *context_m;
    QLabel *tip;
    QGraphicsScene *scene;

    int timer4ai;
    QNetworkAccessManager manager;
    QHash<QUrl,std::function<void (const QJsonObject &obj)>> dump;
    QString faceset_id;
};

#endif // MAINWINDOW_H
