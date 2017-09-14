#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include "deviceconfig.h"
#include <QMouseEvent>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void mouseReleaseEvent(QMouseEvent *event)  Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event)  Q_DECL_OVERRIDE;

public slots:
    void addDevice();
    void togglePage();
private slots:
    void on_player_incoming(const QImage &);

private:
    Ui::MainWindow *ui;
    QMenu *context_m;

};

#endif // MAINWINDOW_H
