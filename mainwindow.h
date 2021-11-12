#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QModbusPdu>
#include <QTime>

#include "ui_interface.h"
#include "modbustcpclient.h"
#include "modbustcpclient_p.h"
#include "mymodbus.h"
#include "mycamera.h"
#include "mycontroller.h"
#include "CameraParams.h"
#include "MvCamera.h"

QT_BEGIN_NAMESPACE
class QModbusClient;
QT_END_NAMESPACE;

//将debug的内容绑定到主窗口的控件上，会存在子线程debug的时候传参的错误
//class DebugHandler
//{
//public:
//    explicit DebugHandler(QtMessageHandler newMessageHandler)
//        : oldMessageHandler(qInstallMessageHandler(newMessageHandler))
//    {}
//    ~DebugHandler() { qInstallMessageHandler(oldMessageHandler); }

//private:
//    QtMessageHandler oldMessageHandler;
//};

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
    friend class ModbusTcpClient;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow *instance();
    //    void appendToLog(const QString &msg) {
    ////        logTextEdit->appendPlainText(msg);
    //    }

signals:
    /**********************发送UI界面设置的TCP参数给子线程*************************/
    void sendTimeOut(int);
    void sendNumberOfRetries(int);
    void sendTcpPort(QString str);
    void sendTcpAddress(QString);

    /**********************从主线程发送数据给子线程的相关信号*************************/
    void sendThreePointList(QStringList);
    void sendLinePointsList(QStringList);
    void sendSixAxisForceSensorData(QList<float>);
    void masterValveEnableSignal();
    void masterValveUnableSignal();
    void sendCurrentDeviceIndex(int);//获取设备下拉列表的index

public slots:

    /***************************************************************
    **************************modbus槽函数****************************
    ****************************************************************/

    /************************TCP连接相关参数设置*************************/
    void on_retriesSpin_valueChanged();                      //UI界面重试次数
    void on_timeoutSpin_valueChanged();                      //UI界面重试超时
    void on_tcpPortEdit_textChanged();                       //UI界面端口
    void on_tcpAddressEdit_textChanged();                    //UI界面IP地址

    /**************************线程初始化******************************/
    void buttonInit();
    void modbusInit();
    void cameraInit();
    void controllerInit();

    /*********************传感器数据存储相关函数**************************/
    void on_pushButton_saveDirectory_clicked();//存储目录
    void on_pushButton_saveOnce_clicked();//单次存储
    void on_pushButton_saveCont_clicked();//连续存储
    void on_pushButton_stopSave_clicked();//停止存储

    /************************三点连发相关函数****************************/
    void on_pushButton_getThreePointsFileDir_clicked();
    void on_pushButton_getThreePoints_clicked();

    /**************************点序列相关函数***************************/
    void on_pushButton_getPointsSequenceDirectory_clicked();
    void on_pushButton_getPointsSequence_clicked();

    /***************************功能函数*******************************/
    void data_show(QModbusReply *reply);


    /***************************************************************
    ***************************camera槽函数***************************
    ****************************************************************/

    /****************************提示窗口*****************************/
    void showErrorMessage(QString error_massage);
    void addComboBoxCameraDeviceItems(QString itemString);

    void displayOneFrame(MV_FRAME_OUT stImageInfo,MV_DISPLAY_FRAME_INFO stDisplayInfo);


private slots:

private:
    //    DebugHandler m_debugHandler;
    QModbusClient *m_device = nullptr;

    //文件目录获取函数
    QString directory_save               = QStringLiteral("C:/Users/Loki_/Desktop/zhuqu.txt");
    QString directory_get_three_points   = QStringLiteral("C:/Users/Loki_/Desktop/joints.txt");
    QString directory_get_line_points    = QStringLiteral("C:/Users/Loki_/Desktop/joints_theta_all.txt");

    //连续存储定时器
    QTimer *timer_save                   = new QTimer(this);

    //多点连发
    QStringList three_points_list;

    //连续发送一条直线上的点
    int line_points_index                = 0;

    CMvCamera *myMvCamera = new CMvCamera;

};

#endif // MAINWINDOW_H
