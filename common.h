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

#define WEBCAM_HOST "192.168.1.1"
#define WEBCAM_PORT 9999
#define WEBCAM_USER "root"
#define WEBCAM_CODE "xxxxxxxx"

#define app_key "M40GhBvM06XR0S8mnmr761u4j82m2030Ig5"
#define app_secret "sJe3YecdSOenmmmw292P7muPrkt3079e8pM"
#define CM_HOST "0.0.0.0:9988"

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
