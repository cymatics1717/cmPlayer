#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QJsonObject>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include "deviceconfig.h"
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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

    void post();
    void addPixmap(QJsonObject obj);
    void onFinished(QNetworkReply*);
    void onError(QNetworkReply::NetworkError);
private slots:

private:
    Ui::MainWindow *ui;
    QMenu *context_m;
    QLabel *tip;
    QGraphicsScene *scene;

    int timerID;
    QNetworkAccessManager manager;
};

#endif // MAINWINDOW_H
