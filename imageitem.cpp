#include "imageitem.h"

#include <QStaticText>

imageItem::imageItem(const QPixmap &pixmap, QGraphicsItem *p) :
    QObject(nullptr),QGraphicsPixmapItem(pixmap.scaledToWidth(250),p)
{
//    setFlag(QGraphicsItem::ItemIsSelectable);
//    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setCacheMode(DeviceCoordinateCache);
}

void imageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter,option,widget);
    painter->drawText(100,100,"hello");
}
