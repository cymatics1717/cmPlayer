#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include <QDialog>
#include "common.h"

namespace Ui {
class deviceConfig;
}

class deviceConfig : public QDialog
{
    Q_OBJECT

public:
    explicit deviceConfig(QWidget *parent = 0);
    ~deviceConfig();

    QString aliasName() const;
    QString ip() const;
    quint64 port() const;
    QString username() const;
    QString password() const;
private:
    Ui::deviceConfig *ui;
    QStringList devlist;
    QList<userInfo> devices;
};

#endif // DEVICECONFIG_H
