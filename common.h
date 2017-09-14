#ifndef COMMON_H
#define COMMON_H
#include <QtGlobal>

typedef struct _userInfo
{
    QString alias;
    QString ip;
    quint64 port;
    QString name;
    QString password;
} userInfo;


#endif // COMMON_H
