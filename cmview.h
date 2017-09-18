#ifndef CMVIEW_H
#define CMVIEW_H

#include <QGraphicsView>

class cmView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit cmView(QWidget *p = nullptr);
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event)  Q_DECL_OVERRIDE;
};

#endif // CMVIEW_H
