#include "cmview.h"
#include <QDebug>
#include <QMouseEvent>
cmView::cmView(QWidget *p):QGraphicsView(p)
{
}

void cmView::resizeEvent(QResizeEvent *event)
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(event);
}

//void cmView::mousePressEvent(QMouseEvent *event)
//{
//    qDebug() << sceneRect() <<mapToScene(event->pos()) ;
//    qDebug() << scene()->items();
//    QGraphicsView::mousePressEvent(event);
//}
