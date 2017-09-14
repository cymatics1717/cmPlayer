#include "hkplayer.h"
#include <QDateTime>
#include <QDebug>
#include <QPainter>

#include "PlayM4.h"
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include "opencv/highgui.h"

static LONG nPort = -1;
static HKPlayer *self = NULL;

HKPlayer::HKPlayer(QWidget *parent) : QWidget(parent)
  ,lUserID(-1),channelID(1),sdktag(false)
{

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
}

void HKPlayer::init()
{
    logo.load(":/images/login-logo.png");
    setContextMenuPolicy(Qt::ActionsContextMenu);
    context_m = new QMenu(this);
    context_m->addAction("capture a shot",this,SLOT(capture()));
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

    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
}

void HKPlayer::capture()
{

//    if(NET_DVR_Init()){
//        int err = NET_DVR_GetLastError();
//        qDebug()<< NET_DVR_GetErrorMsg(&err);
//    } else {
//        qDebug()<< "failed to init HK SDK";
//    }

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

static QImage matToQImage( const cv::Mat &inMat )
{
   switch ( inMat.type() )
   {
      // 8-bit, 4 channel
      case CV_8UC4:
      {
         QImage image( inMat.data,
                       inMat.cols, inMat.rows,
                       static_cast<int>(inMat.step),
                       QImage::Format_ARGB32 );

         return image;
      }

      // 8-bit, 3 channel
      case CV_8UC3:
      {
         QImage image( inMat.data,
                       inMat.cols, inMat.rows,
                       static_cast<int>(inMat.step),
                       QImage::Format_RGB888 );

         return image.rgbSwapped();
      }

      // 8-bit, 1 channel
      case CV_8UC1:
      {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
         QImage image( inMat.data,
                       inMat.cols, inMat.rows,
                       static_cast<int>(inMat.step),
                       QImage::Format_Grayscale8 );
#else
         static QVector<QRgb>  sColorTable;

         // only create our color table the first time
         if ( sColorTable.isEmpty() )
         {
            sColorTable.resize( 256 );

            for ( int i = 0; i < 256; ++i )
            {
               sColorTable[i] = qRgb( i, i, i );
            }
         }

         QImage image( inMat.data,
                       inMat.cols, inMat.rows,
                       static_cast<int>(inMat.step),
                       QImage::Format_Indexed8 );

         image.setColorTable( sColorTable );
#endif

         return image;
      }

      default:
         qWarning() << "cv::Mat image type not handled in switch:" << inMat.type();
         break;
   }

   return QImage();
}

static void CALLBACK DecCBFun(int nPort,char * pBuf,int nSize,FRAME_INFO * pFrameInfo, void* nReserved1,int nReserved2)
{
    qDebug("TYPE:%d-[%d*%d]",pFrameInfo->nType,pFrameInfo->nWidth,pFrameInfo->nHeight);
    switch (pFrameInfo->nType) {
    case T_YV12:
    {
        static int cnt=0;
        cv::Mat dst(pFrameInfo->nHeight,pFrameInfo->nWidth,CV_8UC3);
        cv::Mat src(pFrameInfo->nHeight + pFrameInfo->nHeight/2,pFrameInfo->nWidth,CV_8UC1,(uchar*)pBuf);
        cv::cvtColor(src,dst,CV_YUV2BGR_YV12);

//        cv::imwrite(QString("/home/wayne/test%1.jpg").arg(++cnt).toStdString(),dst);
//        QImage image = matToQImage(dst);
//        image.save(QString("/home/wayne/test%1.jpg").arg(++cnt));
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
    case NET_DVR_SYSHEAD:    //系统头
        if (!PlayM4_GetPort(&nPort)) //获取播放库未使用的通道号
        {
            break;
        }
        if(dwBufSize > 0)
        {
            if (!PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME))  //设置实时流播放模式
            {
                break;
            }
            if (!PlayM4_OpenStream(nPort,pBuffer,dwBufSize,1024*1024))//打开流接口
            {
                dRet=PlayM4_GetLastError(nPort);
                break;
            }
            //设置解码回调函数 只解码不显示
            if (!PlayM4_SetDecCallBack(nPort,DecCBFun))
            {
              dRet=PlayM4_GetLastError(nPort);
              break;
            }

//            //设置解码回调函数 解码且显示在窗口句柄中
//            if (!PlayM4_SetDecCallBackEx(nPort,DecCBFun,NULL,NULL))
//            {
//                dRet=PlayM4_GetLastError(nPort);
//                break;
//            }
            if (!PlayM4_SetDisplayBuf(nPort, 5))//设置缓冲区大小,2-5
            {
                break;
            }
//            if (!PlayM4_Play(nPort,self->alien->winId()))
            if (!PlayM4_Play(nPort,NULL))
            {
                dRet=PlayM4_GetLastError(nPort);
                break;
            }
            //打开音频解码, 需要码流是复合流
            if (!PlayM4_PlaySound(nPort))
            {
                dRet=PlayM4_GetLastError(nPort);
                break;
            }
        }
        break;

    case NET_DVR_STREAMDATA:   //码流数据
        if (dwBufSize > 0 && nPort != -1)
        {
            BOOL inData = PlayM4_InputData(nPort,pBuffer,dwBufSize);
            while (!inData)
            {
//                Sleep(10);
                inData=PlayM4_InputData(nPort,pBuffer,dwBufSize);
                qDebug("PlayM4_InputData failed");
            }
        }
        break;
    }
}

void HKPlayer::addDevice(const userInfo &user)
{
    qDebug() <<"login with param:"
            << QStringList({user.alias,user.ip,QString::number(user.port),user.name,user.password});
    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    lUserID = NET_DVR_Login_V30((char*)user.ip.toStdString().c_str(), user.port,
                                (char*)user.name.toStdString().c_str(),
                                (char*)user.password.toStdString().c_str(), &struDeviceInfo);

    if (lUserID < 0)
    {
        qDebug("Login error, %d", NET_DVR_GetLastError());
        return;
    }
    qDebug("The max number of analog channels: %d",struDeviceInfo.byChanNum); //模拟通道个数
    qDebug("The max number of IP channels: %d",struDeviceInfo.byIPChanNum);//IP 通道个数


    NET_DVR_SetExceptionCallBack_V30(0, NULL,hkExceptionCallBack, NULL);

    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    struPlayInfo.hPlayWnd = (HWND)winId();
    struPlayInfo.lChannel  = channelID;
    struPlayInfo.byPreviewMode = 0;
    struPlayInfo.dwStreamType = 0;
    struPlayInfo.dwLinkMode =0; //0- TCP 方式,1- UDP 方式,2- 多播方式,3- RTP 方式,4-RTP/RTSP,5-RSTP/HTTP
    struPlayInfo.bBlocked = 1;  //0- 非阻塞取流,1- 阻塞取流
    LONG lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, hkRealDataCallBack, NULL);

//    NET_DVR_CLIENTINFO ClientInfo;
//    ClientInfo.lChannel = channelID;
//    ClientInfo.hPlayWnd = (HWND)winId();
//    ClientInfo.lLinkMode = 0;       //Main Stream
//    ClientInfo.sMultiCastIP = NULL;
//    LONG lRealPlayHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo, hkRealDataCallBack, NULL);

    if (lRealPlayHandle < 0)
    {
        qDebug("NET_DVR_RealPlay_V40 error\n");
        removeDevice();
    }
}

void HKPlayer::removeDevice()
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
    QPainter painter(this);
    painter.drawPixmap(width()/2-logo.width()/2,height()/2,logo);
}

void HKPlayer::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        context_m->exec(QCursor::pos());
    }
}

