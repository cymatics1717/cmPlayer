#ifndef HKPLAYER_H
#define HKPLAYER_H

#include <QMenu>
#include <QWidget>
#include <QMouseEvent>

#include "HCNetSDK.h"
#include "common.h"
class HKPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit HKPlayer(QWidget *parent = nullptr);
    ~HKPlayer();
    void addDevice(const userInfo &user);
    void removeDevice();
    QWidget *alien;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event)  Q_DECL_OVERRIDE;

signals:
    void incoming(const QImage &image);
public slots:
    void draw(const QImage &image);
    void init();
    void initSDK();
    void capture();

private:
    LONG lUserID;
    LONG channelID;
    bool sdktag;
    QPixmap logo;
    QMenu *context_m;
    QImage cap;
};

#endif // HKPLAYER_H
