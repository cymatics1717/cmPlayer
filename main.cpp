#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QString QT_MESSAGE_PATTERN=
            "[%{if-debug}D%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}]"
            ": %{file}:%{line} %{function} - %{message}";
    qSetMessagePattern(QT_MESSAGE_PATTERN);

    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(a.desktop()->screenGeometry().size()*.7);
    w.move(300,100);
    w.show();
//    w.showFullScreen();
    return a.exec();
}
