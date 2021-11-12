#include "mycamera.h"

MyCamera::MyCamera(QObject *parent) : QObject(parent)
{
//    m_hwndDisplay = (HWND)ui.lab_LeftImage->winId();  //分配m_hwndDisplay[0] 给左相机
    //    m_hwndRect = ui.lab_LeftImage->rect();
//    qDebug()<< "camera thread:" << thread();

}

void MyCamera::searchDevice()
{
//    qDebug()<< "camera thread:" << thread();

    //声明变量，用于储存将在combox中显示的内容
    QString strMsg;
    //    m_ctrlDeviceCombo.ResetContent();
    memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // ch:枚举子网内所有设备 | en:Enumerate all devices within subnet
    int nRet = CMvCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);
    if (MV_OK != nRet)
    {
        return;
    }

    // ch:将值加入到信息列表框中并显示出来 | en:Add value to the information list box and display
    for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++)
    {
        MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
        if (NULL == pDeviceInfo)
        {
            continue;
        }

        wchar_t* pUserName = NULL;
        if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
        {
            int nIp1 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
            int nIp2 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
            int nIp3 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
            int nIp4 = (m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

            if (strcmp("", (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName)) != 0)
            {
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
            }
            else
            {
                char strUserName[256] = {0};
                sprintf_s(strUserName, 256, "%s %s (%s)", pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName,
                          pDeviceInfo->SpecialInfo.stGigEInfo.chModelName,
                          pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
            }
            strMsg = QString("[%1]GigE: %2 (%3.%4.%5.%6)").arg(i).arg(pUserName)
                    .arg(nIp1).arg(nIp2).arg(nIp3).arg(nIp4);
        }
        else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
        {
            if (strcmp("", (char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName) != 0)
            {
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
            }
            else
            {
                char strUserName[256] = {0};
                sprintf_s(strUserName, 256, "%s %s (%s)", pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName,
                          pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName,
                          pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
            }
            strMsg = QString("[%1]UsbV3: %2").arg(i).arg(pUserName);
        }
        else
        {
            emit sendErrorMeassage("Unknown device enumerated");
        }

        //在主界面设备列表添加可用设备
        emit sendComboBoxCameraDeviceItems(strMsg);

        if (pUserName)
        {
            delete[] pUserName;
            pUserName = NULL;
        }
    }

    if (0 == m_stDevList.nDeviceNum)
    {
        emit sendErrorMeassage("No device");
        return;
    }
    //    comboBox_cameraDevice->setCurrentIndex(0);

    //        EnableControls(TRUE);
}

void MyCamera::cameraOn()
{
    if (TRUE == m_bOpenDevice || NULL != m_pcMyCamera)
    {
        return;
    }
    //        UpdateData(TRUE);

    int nIndex = 0;//设置combox序号以选中设备
    if ((nIndex < 0) | (nIndex >= MV_MAX_DEVICE_NUM))
    {
        emit sendErrorMeassage("Please select device");
        return;
    }

    // ch:由设备信息创建设备实例 | en:Device instance created by device information
    if (NULL == m_stDevList.pDeviceInfo[nIndex])
    {
        emit sendErrorMeassage("Device does not exist");
        return;
    }

    m_pcMyCamera = new CMvCamera;
    if (NULL == m_pcMyCamera)
    {
        return;
    }

    int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);

//    qDebug() << "camera on";

    if (MV_OK != nRet)
    {
        delete m_pcMyCamera;
        m_pcMyCamera = NULL;

        QString erroMsg = generateErrorMessage("Open Fail",nRet);
        emit sendErrorMeassage(erroMsg);
        return;
    }

    m_bOpenDevice = TRUE;

    // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
    if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
    {
        unsigned int nPacketSize = 0;
        nRet = m_pcMyCamera->GetOptimalPacketSize(&nPacketSize);
        if (nRet == MV_OK)
        {
            nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize",nPacketSize);
            if(nRet != MV_OK)
            {
                QString erroMsg = generateErrorMessage("Warning: Set Packet Size fail!",nRet);
                emit sendErrorMeassage(erroMsg);
            }
        }
        else
        {
            QString erroMsg = generateErrorMessage("Warning: Get Packet Size fail!",nRet);
            emit sendErrorMeassage(erroMsg);
        }
    }

//    m_bOpenDevice = TRUE;
//    qDebug() << "11111";
//    getCameraParameter();// ch:获取参数 | en:Get Parameter
//    qDebug() << "22222";
}

void MyCamera::cameraOff()
{

}

bool MyCamera::removeCustomPixelFormats(MvGvspPixelType enPixelFormat)
{
    int nResult = enPixelFormat & MV_GVSP_PIX_CUSTOM;
    if(MV_GVSP_PIX_CUSTOM == nResult)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int MyCamera::grabThreadProcess()
{
    MV_FRAME_OUT stImageInfo = {0};
    MV_DISPLAY_FRAME_INFO stDisplayInfo = {0};
//    MV_FRAME_OUT stImageInfo;
//    MV_DISPLAY_FRAME_INFO stDisplayInfo;
//    int nRet = MV_OK;

    while(m_bThreadState)
    {
        int nRet = m_pcMyCamera->GetImageBuffer(&stImageInfo, 1000);
        if (nRet == MV_OK)
        {
            //用于保存图片
            EnterCriticalSection(&m_hSaveImageMux);
            if (NULL == m_pSaveImageBuf || stImageInfo.stFrameInfo.nFrameLen > m_nSaveImageBufSize)
            {
                if (m_pSaveImageBuf)
                {
                    free(m_pSaveImageBuf);
                    m_pSaveImageBuf = NULL;
                }

                m_pSaveImageBuf = (unsigned char *)malloc(sizeof(unsigned char) * stImageInfo.stFrameInfo.nFrameLen);
                if (m_pSaveImageBuf == NULL)
                {
                    LeaveCriticalSection(&m_hSaveImageMux);
                    return 0;
                }
                m_nSaveImageBufSize = stImageInfo.stFrameInfo.nFrameLen;
            }
//            qDebug()<<"4564891";
            memcpy(m_pSaveImageBuf, stImageInfo.pBufAddr, stImageInfo.stFrameInfo.nFrameLen);
            memcpy(&m_stImageInfo, &(stImageInfo.stFrameInfo), sizeof(MV_FRAME_OUT_INFO_EX));
            LeaveCriticalSection(&m_hSaveImageMux);

            //自定义格式不支持显示
            if(removeCustomPixelFormats(stImageInfo.stFrameInfo.enPixelType))
            {
                m_pcMyCamera->FreeImageBuffer(&stImageInfo);
                continue;
            }

//            stDisplayInfo.hWnd = m_hwndDisplay;
//            stDisplayInfo.pData = stImageInfo.pBufAddr;
//            stDisplayInfo.nDataLen = stImageInfo.stFrameInfo.nFrameLen;
//            stDisplayInfo.nWidth = stImageInfo.stFrameInfo.nWidth;
//            stDisplayInfo.nHeight = stImageInfo.stFrameInfo.nHeight;
//            stDisplayInfo.enPixelType = stImageInfo.stFrameInfo.enPixelType;

//            m_pcMyCamera->DisplayOneFrame(&stDisplayInfo);

            emit sendOneFrameImage(stImageInfo,stDisplayInfo);

            m_pcMyCamera->FreeImageBuffer(&stImageInfo);
        }
        else
        {
            if (MV_TRIGGER_MODE_ON ==  m_nTriggerMode)
            {
                Sleep(5);
            }
        }
    }
    return MV_OK;
}

void MyCamera::getCurrentDeviceIndex(int)
{

}

unsigned int __stdcall grabThread(void* pUser)
{
    if (pUser)
    {
        MyCamera* pCam = (MyCamera*)pUser;

        pCam->grabThreadProcess();

        return 0;
    }

    return -1;
}

void MyCamera::cameraStartGrabbing()
{
    qDebug() << QStringLiteral("开始采集");
    if (FALSE == m_bOpenDevice || TRUE == m_bStartGrabbing || NULL == m_pcMyCamera)
    {
        return;
    }

    memset(&m_stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    unsigned int nThreadID = 0;

//    int nRet = m_pcMyCamera->StartGrabbing();

    m_hGrabThread = (void*)_beginthreadex( NULL , 0 , grabThread , this, 0 , &nThreadID );
    if (NULL == m_hGrabThread)
    {
        m_bThreadState = FALSE;
        QString erroMsg = generateErrorMessage("Create thread fail",0);
        emit sendErrorMeassage(erroMsg);
        return;
    }

//    m_bThreadState = TRUE;
//    grabThreadProcess();

    int nRet = m_pcMyCamera->StartGrabbing();
    if (MV_OK != nRet)
    {
        m_bThreadState = FALSE;
        QString erroMsg = generateErrorMessage("Start grabbing fail",nRet);
        emit sendErrorMeassage(erroMsg);
        return;
    }
    m_bStartGrabbing = TRUE;
}

void MyCamera::cameraStopGrabbing()
{

}

void MyCamera::cameraSoftwareOnceTriggle()
{

}

void MyCamera::setCameraParameter()
{

}

void MyCamera::getCameraParameter()
{
    int nRet = getTriggerMode();
    if (nRet != MV_OK)
    {
        QString erroMsg = generateErrorMessage("Get Trigger Mode Fail",nRet);
        emit sendErrorMeassage(erroMsg);
    }

    nRet = getExposureTime();
    if (nRet != MV_OK)
    {
        QString erroMsg = generateErrorMessage("Get Exposure Time Fail",nRet);
        emit sendErrorMeassage(erroMsg);
    }

    nRet = getGain();
    if (nRet != MV_OK)
    {
        QString erroMsg = generateErrorMessage("Get Gain Fail",nRet);
        emit sendErrorMeassage(erroMsg);
    }

    nRet = getFrameRate();
    if (nRet != MV_OK)
    {
        QString erroMsg = generateErrorMessage("Get Frame Rate Fail",nRet);
        emit sendErrorMeassage(erroMsg);
    }

    nRet = getTriggerSource();
    if (nRet != MV_OK)
    {
        QString erroMsg = generateErrorMessage("Get Trigger Source Fail",nRet);
        emit sendErrorMeassage(erroMsg);
    }

    //    UpdateData(FALSE);
}

void MyCamera::cameraTriggerMode()
{
    //    UpdateData(TRUE);
    emit sendTriggerModeRadioCheckedSignal(TRUE);
    emit sendSoftwareTriggerEnableSignal(TRUE);
    //       ((CButton *)GetDlgItem(IDC_CONTINUS_MODE_RADIO))->SetCheck(FALSE);
    //       ((CButton *)GetDlgItem(IDC_TRIGGER_MODE_RADIO))->SetCheck(TRUE);
    //       ((CButton *)GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK))->EnableWindow(TRUE);
    m_nTriggerMode = MV_TRIGGER_MODE_ON;
    int nRet = setTriggerMode();
    if (MV_OK != nRet)
    {
        QString erroMsg = generateErrorMessage("Set Trigger Mode Fail",nRet);
        emit sendErrorMeassage(erroMsg);
        return;
    }

    if (m_bStartGrabbing == TRUE)
    {
        if (TRUE == m_bSoftWareTriggerCheck)
        {
            emit sendSoftwareOnceEnableSignal(TRUE);
            //               GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON )->EnableWindow(TRUE);
        }
    }
}

void MyCamera::cameraContinusMode()
{
    //    ((CButton *)GetDlgItem(IDC_CONTINUS_MODE_RADIO))->SetCheck(TRUE);
    //    ((CButton *)GetDlgItem(IDC_TRIGGER_MODE_RADIO))->SetCheck(FALSE);
    //    ((CButton *)GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK))->EnableWindow(FALSE);
    emit sendContinusModeRadioCheckedSignal(TRUE);
    emit sendSoftwareTriggerEnableSignal(FALSE);

    m_nTriggerMode = MV_TRIGGER_MODE_OFF;
    int nRet = setTriggerMode();
    if (MV_OK != nRet)
    {
        return;
    }
    emit sendSoftwareOnceEnableSignal(FALSE);
    //    GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(FALSE);
}

int MyCamera::setTriggerMode()
{
    return m_pcMyCamera->SetEnumValue("TriggerMode", m_nTriggerMode);
}

int MyCamera::getTriggerMode()
{
    MVCC_ENUMVALUE stEnumValue = {0};

    int nRet = m_pcMyCamera->GetEnumValue("TriggerMode", &stEnumValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    m_nTriggerMode = stEnumValue.nCurValue;
    if (MV_TRIGGER_MODE_ON ==  m_nTriggerMode)
    {
        cameraTriggerMode();
    }
    else
    {
        m_nTriggerMode = MV_TRIGGER_MODE_OFF;
        cameraContinusMode();
    }

    qDebug()<<"m_nTriggerMode: " << m_nTriggerMode;

    return MV_OK;
}

int MyCamera::setExposureTime()
{
    //    // ch:调节这两个曝光模式，才能让曝光时间生效
    //    // en:Adjust these two exposure mode to allow exposure time effective
    //    int nRet = m_pcMyCamera->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
    //    if (MV_OK != nRet)
    //    {
    //        return nRet;
    //    }

    //    m_pcMyCamera->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);

    //    return m_pcMyCamera->SetFloatValue("ExposureTime", (float)m_dExposureEdit);
    return 0;
}

int MyCamera::getExposureTime()
{
    MVCC_FLOATVALUE stFloatValue = {0};

    int nRet = m_pcMyCamera->GetFloatValue("ExposureTime", &stFloatValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    //    m_dExposureEdit = stFloatValue.fCurValue;

    return MV_OK;
}

int MyCamera::setGain()
{
    //    // ch:设置增益前先把自动增益关闭，失败无需返回
    //    //en:Set Gain after Auto Gain is turned off, this failure does not need to return
    //    m_pcMyCamera->SetEnumValue("GainAuto", 0);

    //    return m_pcMyCamera->SetFloatValue("Gain", (float)m_dGainEdit);
    return 0;
}

int MyCamera::getGain()
{
    MVCC_FLOATVALUE stFloatValue = {0};

    int nRet = m_pcMyCamera->GetFloatValue("Gain", &stFloatValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    //        m_dGainEdit = stFloatValue.fCurValue;

    return MV_OK;
}

int MyCamera::setFrameRate()
{
    //    int nRet = m_pcMyCamera->SetBoolValue("AcquisitionFrameRateEnable", true);
    //    if (MV_OK != nRet)
    //    {
    //        return nRet;
    //    }

    //    return m_pcMyCamera->SetFloatValue("AcquisitionFrameRate", (float)m_dFrameRateEdit);
    return 0;

}

int MyCamera::getFrameRate()
{
    MVCC_FLOATVALUE stFloatValue = {0};

    int nRet = m_pcMyCamera->GetFloatValue("ResultingFrameRate", &stFloatValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }
    //    m_dFrameRateEdit = stFloatValue.fCurValue;

    return MV_OK;
}

int MyCamera::setTriggerSource()
{
    int nRet = MV_OK;
    //        if (m_bSoftWareTriggerCheck)
    //        {
    //            m_nTriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;
    //            nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_nTriggerSource);
    //            if (MV_OK != nRet)
    //            {
    //                ShowErrorMsg(TEXT("Set Software Trigger Fail"), nRet);
    //                return nRet;
    //            }
    //            GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON )->EnableWindow(TRUE);
    //        }
    //        else
    //        {
    //            m_nTriggerSource = MV_TRIGGER_SOURCE_LINE0;
    //            nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_nTriggerSource);
    //            if (MV_OK != nRet)
    //            {
    //                ShowErrorMsg(TEXT("Set Hardware Trigger Fail"), nRet);
    //                return nRet;
    //            }
    //            GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON )->EnableWindow(FALSE);
    //        }

    return nRet;
}

int MyCamera::getTriggerSource()
{
    MVCC_ENUMVALUE stEnumValue = {0};

    int nRet = m_pcMyCamera->GetEnumValue("TriggerSource", &stEnumValue);
    if (MV_OK != nRet)
    {
        return nRet;
    }

    if ((unsigned int)MV_TRIGGER_SOURCE_SOFTWARE == stEnumValue.nCurValue)
    {
        m_bSoftWareTriggerCheck = TRUE;
    }
    else
    {
        m_bSoftWareTriggerCheck = FALSE;
    }

    return MV_OK;
}

QString MyCamera::generateErrorMessage(QString csMessage, long int nErrorNum)
{
    QString errorMsg;
    if (nErrorNum == 0)
    {
        errorMsg = QString("%1").arg(csMessage);
    }
    else
    {
        errorMsg = QString("%1: Error = %2: ").arg(csMessage).arg(nErrorNum);
    }

    switch(nErrorNum)
    {
    case MV_E_HANDLE:           errorMsg += "Error or invalid handle ";                                         break;
    case MV_E_SUPPORT:          errorMsg += "Not supported function ";                                          break;
    case MV_E_BUFOVER:          errorMsg += "Cache is full ";                                                   break;
    case MV_E_CALLORDER:        errorMsg += "Function calling order error ";                                    break;
    case MV_E_PARAMETER:        errorMsg += "Incorrect parameter ";                                             break;
    case MV_E_RESOURCE:         errorMsg += "Applying resource failed ";                                        break;
    case MV_E_NODATA:           errorMsg += "No data ";                                                         break;
    case MV_E_PRECONDITION:     errorMsg += "Precondition error, or running environment changed ";              break;
    case MV_E_VERSION:          errorMsg += "Version mismatches ";                                              break;
    case MV_E_NOENOUGH_BUF:     errorMsg += "Insufficient memory ";                                             break;
    case MV_E_ABNORMAL_IMAGE:   errorMsg += "Abnormal image, maybe incomplete image because of lost packet ";   break;
    case MV_E_UNKNOW:           errorMsg += "Unknown error ";                                                   break;
    case MV_E_GC_GENERIC:       errorMsg += "General error ";                                                   break;
    case MV_E_GC_ACCESS:        errorMsg += "Node accessing condition error ";                                  break;
    case MV_E_ACCESS_DENIED:	errorMsg += "No permission ";                                                   break;
    case MV_E_BUSY:             errorMsg += "Device is busy, or network disconnected ";                         break;
    case MV_E_NETER:            errorMsg += "Network error ";                                                   break;
    }

    //    MessageBox(errorMsg, TEXT("PROMPT"), MB_OK | MB_ICONWARNING);
    return errorMsg;
}
