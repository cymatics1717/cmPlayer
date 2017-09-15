#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include <QDialog>
#include "common.h"
#include "HCNetSDK.h"

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

    QHash<QString,NET_DVR_DEVICEINFO_V30*> devices;

    QStringList devlist;

};

#endif // DEVICECONFIG_H
