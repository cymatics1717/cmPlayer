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

#define WEBCAM_HOST "192.168.0.2"
#define WEBCAM_PORT 80
#define WEBCAM_USER "admin"
#define WEBCAM_CODE "1234abcd"

#define app_key " "
#define app_secret " "
#define CM_HOST ""

#define CM_FACESET_REGISTER "/api/v1/faceset/register"
#define CM_FACESET_REMOVE "/api/v1/faceset/remove"
#define CM_FACESET_LIST "/api/v1/faceset/list"

#define CM_FACE_ADD "/api/v1/face/add"
#define CM_FACE_RECOG "/api/v1/face/recog/"
#define CM_FACE_SET_NAME "/api/v1/face/set_name"
#define CM_FACE_LIST "/api/v1/face/list"
#define CM_FACE_REMOVE "/api/v1/face/remove"

#define CM_DETECT_CAR_PLATE "/api/v1/detect/carplate"

#endif // COMMON_H
