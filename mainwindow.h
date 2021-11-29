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
#include "commanderInterface.h"

QT_BEGIN_NAMESPACE
class QModbusClient;
QT_END_NAMESPACE;

//��debug�����ݰ󶨵������ڵĿؼ��ϣ���������߳�debug��ʱ�򴫲εĴ���
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
    /**********************����UI�������õ�TCP���������߳�*************************/
    void sendTimeOut(int);
    void sendNumberOfRetries(int);
    void sendTcpPort(QString str);
    void sendTcpAddress(QString);

    /**********************�����̷߳������ݸ����̵߳�����ź�*************************/
    void sendThreePointList(QStringList);
    void sendLinePointsList(QStringList);
    void sendSixAxisForceSensorData(QList<float>);
    void masterValveEnableSignal();
    void masterValveUnableSignal();
    void sendCurrentDeviceIndex(int);//��ȡ�豸�����б��index
    void sendDataToController(bool systemStatus,JointTheta jointTheta);
    void sendCurrentJointsAngles(double*);

public slots:

    /***************************************************************
    **************************modbus�ۺ���****************************
    ****************************************************************/

    /************************TCP������ز�������*************************/
    void on_retriesSpin_valueChanged();                      //UI�������Դ���
    void on_timeoutSpin_valueChanged();                      //UI�������Գ�ʱ
    void on_tcpPortEdit_textChanged();                       //UI����˿�
    void on_tcpAddressEdit_textChanged();                    //UI����IP��ַ

    /**************************�̳߳�ʼ��******************************/
    void buttonInit();
    void modbusInit();
    void cameraInit();
    void controllerInit();

    /*********************���������ݴ洢��غ���**************************/
    void on_pushButton_saveDirectory_clicked();//�洢Ŀ¼
    void on_pushButton_saveOnce_clicked();//���δ洢
    void on_pushButton_saveCont_clicked();//�����洢
    void on_pushButton_stopSave_clicked();//ֹͣ�洢

    /************************����������غ���****************************/
    void on_pushButton_getThreePointsFileDir_clicked();
    void on_pushButton_getThreePoints_clicked();

    /**************************��������غ���***************************/
    void on_pushButton_getPointsSequenceDirectory_clicked();
    void on_pushButton_getPointsSequence_clicked();

    /***************************���ܺ���*******************************/
    void readDataProcess(bool,quint16,QModbusReply *reply);
    void readRegesiterProcess(QModbusReply *replyRegister);
    void saveDataRead();
    void saveDataToWrite(double[]);
    void saveDataWritten(double[]);


    /***************************************************************
    ***************************camera�ۺ���***************************
    ****************************************************************/

    /****************************��ʾ����*****************************/
    void showErrorMessage(QString error_massage);
    void addComboBoxCameraDeviceItems(QString itemString);
    void displayOneFrame(MV_FRAME_OUT stImageInfo,MV_DISPLAY_FRAME_INFO stDisplayInfo);


private slots:

    void on_pushButton_getTrajectorySaveDirectory_clicked();

private:
    //    DebugHandler m_debugHandler;
    QModbusClient *m_device = nullptr;

    //�ļ�Ŀ¼��ȡ����
    QString directory_save               =      QStringLiteral("E:/QtWorkSpace/Work/DeepSeaManipulator/files/record.txt");
    QString directory_get_three_points   =      QStringLiteral("E:/QtWorkSpace/Work/DeepSeaManipulator/files/joints.txt");
    QString directory_get_line_points    =      QStringLiteral("E:/QtWorkSpace/Work/DeepSeaManipulator/files/joints_theta_all.txt");
    QString directory_save_trajectory    =      QStringLiteral("E:/QtWorkSpace/Work/DeepSeaManipulator/files/trajectoryPlan.txt");
    QString directory_data_read          =      QStringLiteral("E:/QtWorkSpace/Work/DeepSeaManipulator/files/data_read.txt");


    //�����洢��ʱ��
    QTimer *timer_save                   =      new QTimer(this);

    //�������
    QStringList three_points_list;

    //��������һ��ֱ���ϵĵ�
    int line_points_index                =      0;

    CMvCamera *myMvCamera = new CMvCamera;
    double current_joints_angles[6];

};

#endif // MAINWINDOW_H
