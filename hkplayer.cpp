#include "hkplayer.h"
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QThread>

#include "PlayM4.h"
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include "opencv/highgui.h"

static HKPlayer *self = NULL;

HKPlayer::HKPlayer(QWidget *parent) :
    QWidget(parent),device(nullptr),user(nullptr),lUserID(-1),nPort(-1),sdktag(false)
{
//    setMouseTracking(true);
}

HKPlayer::~HKPlayer()
{
    if(sdktag){
        qDebug()<< "step to cleanup SDK context.";
        if(NET_DVR_Cleanup()){
            qDebug()<< "bye.";
        } else {
            int err = NET_DVR_GetLastError();
            qDebug()<<"failed to cleanup SDK"<< NET_DVR_GetErrorMsg(&err);
        }
    }

    delete device;
    delete user;
}

void HKPlayer::init()
{
    logo.load(":/images/login-logo.png");
    setContextMenuPolicy(Qt::ActionsContextMenu);
    context_m = new QMenu(this);
    context_m->addAction("&capture a shot",this,SLOT(capture()));
    context_m->addAction("&get camera local time",this,SLOT(getTime()));
    self = this;

    initSDK();
}

void HKPlayer::initSDK()
{
    qDebug()<< "step to init HK SDK";
    sdktag = NET_DVR_Init();
    if(sdktag){
        int err = NET_DVR_GetLastError();
        qDebug()<< NET_DVR_GetErrorMsg(&err);
    } else {
        qDebug()<< "failed to init HK SDK";
    }
}

void HKPlayer::capture()
{
    if(NET_DVR_SetCapturePictureMode(JPEG_MODE)
            &&NET_DVR_CapturePicture(lRealPlayHandle,(char*)QString("./hk%1.jpg")
              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss_zzz"))
                                     .toStdString().c_str())){
        int err = NET_DVR_GetLastError();
        qDebug()<< NET_DVR_GetErrorMsg(&err);
    } else {
        qDebug()<< "failed to set and capture jpeg";
    }
}

void HKPlayer::setTime(){
    NET_DVR_TIME tm;
    QDateTime current = QDateTime::currentDateTime();

    tm.dwYear = current.date().year();
    tm.dwMonth = current.date().month();
    tm.dwDay = current.date().day();
    tm.dwHour = current.time().hour();
    tm.dwMinute = current.time().minute();
    tm.dwSecond = current.time().second();

    if (!NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_TIMECFG, 0xFFFFFFFF, &tm,sizeof(NET_DVR_TIME)))
    {
        qDebug("NET_DVR_SET_TIME error, %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
    }
}

void HKPlayer::openSound()
{
    if(NET_DVR_OpenSound_Card(lRealPlayHandle)
//            &&NET_DVR_AudioPreview_Card(lRealPlayHandle, TRUE)
            &&NET_DVR_SetVolume_Card(lRealPlayHandle, 0xffff)){
        qDebug()<< "success to open sound.";
    } else {
        int err = NET_DVR_GetLastError();
        qDebug()<< NET_DVR_GetErrorMsg(&err);
    }
}

void HKPlayer::getTime()
{
//    setTime();
    QString msg ;
    NET_DVR_TIME tm;
    if (NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_TIMECFG,0xFFFFFFFF,&tm,sizeof(NET_DVR_TIME),LPDWORD(&tm)))
    {
        msg = QString("local time: %1-%2-%3 %4:%5:%6").arg(tm.dwYear)
                .arg(tm.dwMonth).arg(tm.dwDay)
                .arg(tm.dwHour,2,10,QChar('0'))
                .arg(tm.dwMinute,2,10,QChar('0')).arg(tm.dwSecond,2,10,QChar('0'));
    }
    else
    {
        int err = NET_DVR_GetLastError();
        msg = NET_DVR_GetErrorMsg(&err);
    }
    QMessageBox::information(0,"remote machine time",msg);
}

static void CALLBACK hkExceptionCallBack(DWORD dwType,LONG , LONG , void *)
{
    switch(dwType)
    {
    case EXCEPTION_RECONNECT:
        qDebug("reconnect--------%d\n", QDateTime::currentDateTime().toString());
        break;
    default:
        break;
    }
}

static QImage matToQImage( const cv::Mat &mat )
{
   switch (mat.type())
   {
      case CV_8UC4:
      {
         QImage image( mat.data,
                       mat.cols, mat.rows,
                       static_cast<int>(mat.step),
                       QImage::Format_ARGB32 );

         return image;
      }

      case CV_8UC3:
      {
         QImage image( mat.data,
                       mat.cols, mat.rows,
                       static_cast<int>(mat.step),
                       QImage::Format_RGB888 );

         return image.rgbSwapped();
      }

      case CV_8UC1:
      {
         QImage image( mat.data,
                       mat.cols, mat.rows,
                       static_cast<int>(mat.step),
                       QImage::Format_Grayscale8 );
         return image;
      }

      default:
         qWarning() << "cv::Mat image type not handled in switch:" << mat.type();
         break;
   }

   return QImage();
}

static void updateImage(const cv::Mat &dst){
//    static int cnt=0;
//    cv::imwrite(QString("/home/wayne/test%1.jpg").arg(++cnt).toStdString(),dst);
    QMutexLocker(&self->mutex);
    cv::Mat edges;
    cv::cvtColor(dst, edges, CV_BGR2GRAY);
    cv::adaptiveThreshold(edges, edges,255,CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY,5,6);
    cv::Canny(edges, edges, 0, 1);

    QImage image = matToQImage(edges);
    self->incoming(image);

    QThread::msleep(10);
//    image.save(QString("/home/wayne/test%1.jpg").arg(++cnt));
}

static void CALLBACK DecCBFun(int nPort,char * pBuf,int nSize,FRAME_INFO * pFrameInfo, void* nReserved1,int nReserved2)
{
//    qDebug("TYPE:%d-[%d*%d]",pFrameInfo->nType,pFrameInfo->nWidth,pFrameInfo->nHeight);
    switch (pFrameInfo->nType) {
    case T_YV12:
    {
        cv::Mat dst(pFrameInfo->nHeight,pFrameInfo->nWidth,CV_8UC3);
        cv::Mat src(pFrameInfo->nHeight + pFrameInfo->nHeight/2,pFrameInfo->nWidth,CV_8UC1,(uchar*)pBuf);
        cv::cvtColor(src,dst,CV_YUV2BGR_YV12);
        updateImage(dst);
    }
        break;
    case T_AUDIO8:
    case T_AUDIO16:

        break;
    default:
        break;
    }
}

static void CALLBACK hkRealDataCallBack(LONG lRealHandle,DWORD dwDataType,BYTE *pBuffer,DWORD  dwBufSize, void* dwUser)
{
    if (dwUser != NULL)
    {
        qDebug("Demmo lRealHandle[%d]: Get StreamData! Type[%d], BufSize[%d], pBuffer:%p\n", lRealHandle, dwDataType, dwBufSize, pBuffer);
    }

    DWORD dRet;
    switch (dwDataType)
    {
    case NET_DVR_SYSHEAD:
        if (!PlayM4_GetPort(&self->nPort))
        {
            break;
        }
        if(dwBufSize > 0)
        {
            if (!PlayM4_SetStreamOpenMode(self->nPort, STREAME_REALTIME))
            {
                break;
            }
            if (!PlayM4_OpenStream(self->nPort,pBuffer,dwBufSize,1024*1024))
            {
                dRet=PlayM4_GetLastError(self->nPort);
                break;
            }
            //设置解码回调函数 只解码不显示
            if (!PlayM4_SetDecCallBack(self->nPort,DecCBFun))
            {
              dRet=PlayM4_GetLastError(self->nPort);
              break;
            }

//            //设置解码回调函数 解码且显示在窗口句柄中
//            if (!PlayM4_SetDecCallBackEx(nPort,DecCBFun,NULL,NULL))
//            {
//                dRet=PlayM4_GetLastError(nPort);
//                break;
//            }
            if (!PlayM4_SetDisplayBuf(self->nPort, 5))
            {
                break;
            }
//            if (!PlayM4_Play(nPort,self->alien->winId()))
            if (!PlayM4_Play(self->nPort,NULL))
            {
                dRet=PlayM4_GetLastError(self->nPort);
                break;
            }
            if (!PlayM4_PlaySound(self->nPort))
            {
                dRet=PlayM4_GetLastError(self->nPort);
                break;
            }
        }
        break;

    case NET_DVR_STREAMDATA:
        if (dwBufSize > 0 && self->nPort != -1)
        {
            BOOL inData = PlayM4_InputData(self->nPort,pBuffer,dwBufSize);
            while (!inData)
            {
//                Sleep(10);
                inData=PlayM4_InputData(self->nPort,pBuffer,dwBufSize);
                qDebug("PlayM4_InputData failed");
            }
        }
        break;
    }
}

void HKPlayer::addDevice(const userInfo &uinfo)
{
    qDebug()<<"login:"<< QStringList({uinfo.alias,uinfo.ip,QString::number(uinfo.port),uinfo.name,uinfo.password});

    user = new NET_DVR_USER_LOGIN_INFO;

    user->bUseAsynLogin = 0;
    strncpy(user->sDeviceAddress, uinfo.ip.toStdString().c_str(),NET_DVR_DEV_ADDRESS_MAX_LEN);
    user->wPort = uinfo.port;
    strncpy(user->sUserName, uinfo.name.toStdString().c_str(),NET_DVR_LOGIN_USERNAME_MAX_LEN);
    strncpy(user->sPassword, uinfo.password.toStdString().c_str(),NET_DVR_LOGIN_PASSWD_MAX_LEN);

    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
    NET_DVR_SetExceptionCallBack_V30(0, NULL,hkExceptionCallBack, NULL);

    device = new NET_DVR_DEVICEINFO_V40;
    lUserID = NET_DVR_Login_V40(user, device);

    if (lUserID < 0)
    {
        qDebug("Login error, %d", NET_DVR_GetLastError());
        return;
    }
    qDebug("The max number of analog channels: %d",device->struDeviceV30.byChanNum);
    qDebug("The max number of IP channels: %d",device->struDeviceV30.byIPChanNum);

//    DWORD uiReturnLen;
//    NET_DVR_COMPRESSIONCFG struParams = {0};
//    int iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_COMPRESSCFG, device->struDeviceV30.byStartChan,
//                                &struParams, sizeof(NET_DVR_COMPRESSIONCFG), &uiReturnLen);
//    if (!iRet)
//    {
//        printf("pyd---NET_DVR_GetDVRConfig NET_DVR_GET_COMPRESSCFG_V30 error.\n");
//        logout();
//    }
//    else{
//        printf("NET_DVR_GetDVRConfig done\n");
//    }
    play();
}

void HKPlayer::play(){

    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    struPlayInfo.hPlayWnd = (HWND)winId();
    struPlayInfo.lChannel  = device->struDeviceV30.byStartChan;
    struPlayInfo.byPreviewMode = 0;
    struPlayInfo.dwStreamType = 0;
    struPlayInfo.dwLinkMode =4; //0- TCP 方式,1- UDP 方式,2- 多播方式,3- RTP 方式,4-RTP/RTSP,5-RSTP/HTTP
    struPlayInfo.bBlocked = 0;  //0- 非阻塞取流,1- 阻塞取流
    lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, hkRealDataCallBack, NULL);
    if (lRealPlayHandle < 0)
    {
        qDebug("NET_DVR_RealPlay_V40 error\n");
        logout();
    }
    openSound();
}

void HKPlayer::logout()
{
    qDebug()<< "step to remove device with userID = " << lUserID;
    if(NET_DVR_Logout(lUserID)){
        qDebug()<< "bye.";
    } else {
        int err = NET_DVR_GetLastError();
        qDebug()<<"failed to cleanup SDK"<< NET_DVR_GetErrorMsg(&err);
    }
}

void HKPlayer::paintEvent(QPaintEvent *event)
{
    qDebug()<< QDateTime::currentDateTime().toString("HH:mm:ss.zzz");

    QPainter painter(this);
    if(sdktag){
        if(!logo.isNull())
            painter.drawPixmap(width()/2-logo.width()/2,height()/2,logo);
    } else {
        if(!cap.isNull())
            painter.drawImage(width()/2-cap.width()/2,height()/2-cap.height()/2,cap);
//        painter.drawImage(0,0,cap.scaled(width(),height()));
    }
}

void HKPlayer::mousePressEvent(QMouseEvent *event)
{
    qDebug() << event->pos();
}

void HKPlayer::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << event->pos();
    if(event->button() == Qt::RightButton&&sdktag){
        context_m->exec(QCursor::pos());
    }
}

void HKPlayer::draw(const QImage &image)
{
    cap = image;
    update();
}

