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
    /******************������ʾ��Ϣ��UI������ʾ***********************/
    void sendErrorMeassage(QString);
    void sendComboBoxCameraDeviceItems(QString);
    void sendTriggerModeRadioCheckedSignal(bool);
    void sendContinusModeRadioCheckedSignal(bool);
    void sendSoftwareTriggerEnableSignal(bool);
    void sendSoftwareOnceEnableSignal(bool);
    void sendOneFrameImage(MV_FRAME_OUT stImageInfo,MV_DISPLAY_FRAME_INFO stDisplayInfo);

public slots:
    /***************************�������******************************/
    void searchDevice();
    void cameraOn();
    void cameraOff();
    /***************************�������******************************/
    void cameraStartGrabbing();
    void cameraStopGrabbing();
    void cameraSoftwareOnceTriggle();
    void cameraTriggerMode();
    void cameraContinusMode();
    /***************************�������******************************/
    void getCameraParameter();
    void setCameraParameter();
    /*************************����ɼ��߳�*****************************/
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

    /***************************�����ر�����������demo������******************************/

    BOOL                    m_bSoftWareTriggerCheck   = FALSE;
    double                  m_dExposureEdit           = 10000;
    double                  m_dGainEdit               = 0;
    double                  m_dFrameRateEdit          = 20;
    int                     m_nDeviceCombo            = 0;

    BOOL                    m_bOpenDevice             = FALSE;                        // ch:�Ƿ���豸 | en:Whether to open device
    BOOL                    m_bStartGrabbing          = FALSE;                     // ch:�Ƿ�ʼץͼ | en:Whether to start grabbing
    int                     m_nTriggerMode            = MV_TRIGGER_MODE_OFF;                       // ch:����ģʽ | en:Trigger Mode
    int                     m_nTriggerSource          = MV_TRIGGER_SOURCE_SOFTWARE;                     // ch:����Դ | en:Trigger Source

    CMvCamera*              m_pcMyCamera              = NULL;              // ch:CMyCamera��װ�˳��ýӿ� | en:CMyCamera packed commonly used interface
    HWND                    m_hwndDisplay;                        // ch:��ʾ��� | en:Display Handle
    MV_CC_DEVICE_INFO_LIST  m_stDevList;

    CRITICAL_SECTION        m_hSaveImageMux;
    unsigned char*          m_pSaveImageBuf;
    unsigned int            m_nSaveImageBufSize;
    MV_FRAME_OUT_INFO_EX    m_stImageInfo;

    void*                   m_hGrabThread;              // ch:ȡ���߳̾�� | en:Grab thread hand
    BOOL                    m_bThreadState;

};

#endif // MYCAMERA_H
