#ifndef MYCAMERA_H
#define MYCAMERA_H

#include <QObject>
#include <QDebug>
#include <Windows.h>
#include <process.h>

#include "MvCamera.h"
#include "MvErrorDefine.h"
#include "CameraParams.h"

class MyCamera : public QObject
{
    Q_OBJECT
public:
    explicit MyCamera(QObject *parent = nullptr);

signals:
    /******************发送提示信息给UI界面显示***********************/
    void sendErrorMeassage(QString);
    void sendComboBoxCameraDeviceItems(QString);
    void sendTriggerModeRadioCheckedSignal(bool);
    void sendContinusModeRadioCheckedSignal(bool);
    void sendSoftwareTriggerEnableSignal(bool);
    void sendSoftwareOnceEnableSignal(bool);
    void sendOneFrameImage(MV_FRAME_OUT stImageInfo,MV_DISPLAY_FRAME_INFO stDisplayInfo);

public slots:
    /***************************相机连接******************************/
    void searchDevice();
    void cameraOn();
    void cameraOff();
    /***************************相机功能******************************/
    void cameraStartGrabbing();
    void cameraStopGrabbing();
    void cameraSoftwareOnceTriggle();
    void cameraTriggerMode();
    void cameraContinusMode();
    /***************************相机参数******************************/
    void getCameraParameter();
    void setCameraParameter();
    /*************************相机采集线程*****************************/
    int grabThreadProcess();
    void getCurrentDeviceIndex(int);

private slots:
    int setTriggerMode();
    int getTriggerMode();
    int setExposureTime();
    int getExposureTime();
    int setGain();
    int getGain();
    int setFrameRate();
    int getFrameRate();
    int setTriggerSource();
    int getTriggerSource();
    QString generateErrorMessage(QString csMessage,long int nErrorNum);
    bool removeCustomPixelFormats(enum MvGvspPixelType enPixelFormat);

private:

    /***************************相机相关变量声明，用demo给出的******************************/

    BOOL                    m_bSoftWareTriggerCheck   = FALSE;
    double                  m_dExposureEdit           = 10000;
    double                  m_dGainEdit               = 0;
    double                  m_dFrameRateEdit          = 20;
    int                     m_nDeviceCombo            = 0;

    BOOL                    m_bOpenDevice             = FALSE;                        // ch:是否打开设备 | en:Whether to open device
    BOOL                    m_bStartGrabbing          = FALSE;                     // ch:是否开始抓图 | en:Whether to start grabbing
    int                     m_nTriggerMode            = MV_TRIGGER_MODE_OFF;                       // ch:触发模式 | en:Trigger Mode
    int                     m_nTriggerSource          = MV_TRIGGER_SOURCE_SOFTWARE;                     // ch:触发源 | en:Trigger Source

    CMvCamera*              m_pcMyCamera              = NULL;              // ch:CMyCamera封装了常用接口 | en:CMyCamera packed commonly used interface
    HWND                    m_hwndDisplay;                        // ch:显示句柄 | en:Display Handle
    MV_CC_DEVICE_INFO_LIST  m_stDevList;

    CRITICAL_SECTION        m_hSaveImageMux;
    unsigned char*          m_pSaveImageBuf;
    unsigned int            m_nSaveImageBufSize;
    MV_FRAME_OUT_INFO_EX    m_stImageInfo;

    void*                   m_hGrabThread;              // ch:取流线程句柄 | en:Grab thread hand
    BOOL                    m_bThreadState;

};

#endif // MYCAMERA_H
