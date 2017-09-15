#ifndef HKPLAYER_H
#define HKPLAYER_H

#include <QMenu>
#include <QWidget>
#include <QMouseEvent>
#include <QMutex>

#include "HCNetSDK.h"
#include "common.h"
class HKPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit HKPlayer(QWidget *parent = nullptr);
    ~HKPlayer();
    void addDevice(const userInfo &user);
    void logout();

    QMutex mutex;
    LONG nPort;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event)  Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event)  Q_DECL_OVERRIDE;

signals:
    void incoming(const QImage &image);
    void incoming2(const QImage &image);
public slots:
    void draw(const QImage &image);
    void init();
    void initSDK();
    void play();
    void capture();
    void getTime();
    void setTime();
    void openSound();

private:
    NET_DVR_USER_LOGIN_INFO *user;
    NET_DVR_DEVICEINFO_V40 *device;
    LONG lUserID;
    LONG lRealPlayHandle;
    bool sdktag;
    QPixmap logo;
    QMenu *context_m;
    QImage cap;
};

#endif // HKPLAYER_H
