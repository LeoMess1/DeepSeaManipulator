#include "mainwindow.h"

#include <math.h>
#include <ctime>
#include <QLoggingCategory>
#include <QModbusPdu>
#include <QVector>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
//#include "commander_list_interface.h"

#ifndef QT_STATIC
QT_BEGIN_NAMESPACE
Q_LOGGING_CATEGORY(QT_MODBUS, "qt.modbus")
Q_LOGGING_CATEGORY(QT_MODBUS_LOW, "qt.modbus.lowlevel")
QT_END_NAMESPACE
#endif

QT_USE_NAMESPACE

MainWindow *s_instance = nullptr;

//static void HandlerFunction(QtMsgType, const QMessageLogContext &, const QString &msg)
//{
//    //    将debug信息绑定到logTextEdit
////    if (auto instance = MainWindow::instance())
////        instance->appendToLog(msg);
//}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //m_debugHandler(HandlerFunction)
{
    qDebug()<< "main thread:" << thread();
    setupUi(this);
    s_instance = this;

    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));

    //设置默认目录
    lineEdit_fileSaveAddress->setText(directory_save);
    lineEdit_pointsDir->setText(directory_get_three_points);
    lineEdit_pointsListDir->setText(directory_get_line_points);
    lineEdit_trajectorySaveDirectory->setText(directory_save_trajectory);

    buttonInit();//初始化相关按钮和文本编辑框
    modbusInit();//连接函数初始化
    cameraInit();
    controllerInit();

    on_pushButton_getThreePoints_clicked();
    lineEdit_jointsSpeed->setText("1");


    //    //创建绘图子线程对象
    //    QThread *thread_pic = new QThread;
    //    //创建任务类对象
    //    MyPic *myPic = new MyPic;
    //    //任务对象移动到子线程中
    //    myPic->moveToThread(thread_pic);
    //    //从主线程发送需要绘制的六维力传感器数据
    //    connect(this,&MainWindow::sendSixAxisForceSensorData,myPic,&MyPic::getSixAxisForceSensorData);

    connect(timer_save,&QTimer::timeout,this,&MainWindow::on_pushButton_saveOnce_clicked);//连续存储数据
}

MainWindow::~MainWindow()
{
    s_instance = nullptr;
}

MainWindow *MainWindow::instance()
{
    return s_instance;
}

//初始化相关按钮和文本编辑框
void MainWindow::buttonInit()
{
    //设置连接按钮关联到F1键
    pushButton_connectModbus->setShortcut(Qt::Key_F1);

    //默认执行按钮无效
    pushButton_execute->setEnabled(false);

    //手爪关节
    radioButton_gripperClosedLoopMode->setChecked(true);
    radioButton_gripperOpenLoopMode->setChecked(false);
    pushButton_catch->setEnabled(true);
    pushButton_release->setEnabled(true);
    pushButton_open->setDisabled(true);
    pushButton_close->setDisabled(true);
    connect(radioButton_gripperClosedLoopMode, &QRadioButton::toggled, this, [this](bool toggled) {
        pushButton_catch->setEnabled(toggled);
        pushButton_release->setEnabled(toggled);
        pushButton_open->setDisabled(toggled);
        pushButton_close->setDisabled(toggled);
        ;});

    //切换开环模式，相应改变开环模式下电流输入可用与否
    connect(radioButton_jointsOpenLoopMode, &QRadioButton::toggled, this, [this](bool toggled) {
        doubleSpinBox_joint_1_currentValue->setEnabled(toggled);
        doubleSpinBox_joint_2_currentValue->setEnabled(toggled);
        doubleSpinBox_joint_3_currentValue->setEnabled(toggled);
        doubleSpinBox_Joint_4_currentValue->setEnabled(toggled);
        doubleSpinBox_joint_5_currentValue->setEnabled(toggled);
        doubleSpinBox_joint_6_currentValue->setEnabled(toggled);
        doubleSpinBox_joint_7_currentValue->setEnabled(toggled);
        checkBox_joint_1_possitiveCurrent->setEnabled(toggled);
        checkBox_joint_2_possitiveCurrent->setEnabled(toggled);
        checkBox_joint_3_possitiveCurrent->setEnabled(toggled);
        checkBox_joint_4_possitiveCurrent->setEnabled(toggled);
        checkBox_joint_5_possitiveCurrent->setEnabled(toggled);
        checkBox_joint_6_possitiveCurrent->setEnabled(toggled);
        checkBox_joint_7_possitiveCurrent->setEnabled(toggled);});

    //切换闭环模式，相应改变闭环模式下角度输入可用与否
    connect(radioButton_jointsClosedLoopMode, &QRadioButton::toggled, this, [this](bool toggled) {
        doubleSpinBox_joint_1_angle->setEnabled(toggled);
        doubleSpinBox_joint_2_angle->setEnabled(toggled);
        doubleSpinBox_joint_3_angle->setEnabled(toggled);
        doubleSpinBox_joint_4_angle->setEnabled(toggled);
        doubleSpinBox_joint_5_angle->setEnabled(toggled);
        doubleSpinBox_joint_6_angle->setEnabled(toggled);
        doubleSpinBox_joint_7_angle->setEnabled(toggled);});

    //先触发一次激活开环模式下的设置，再触发闭环模式
    //这样会使得开环模式下的toggle为false，闭关模式下toggle为true
    radioButton_jointsOpenLoopMode->toggle();
    radioButton_jointsClosedLoopMode->toggle();

    //默认每个关节使能
    checkBox_joint_1_enable->setChecked(true);
    checkBox_joint_2_enable->setChecked(true);
    checkBox_joint_3_enable->setChecked(true);
    checkBox_joint_4_enable->setChecked(true);
    checkBox_joint_5_enable->setChecked(true);
    checkBox_joint_6_enable->setChecked(true);
    checkBox_joint_7_enable->setChecked(true);

    //开环模式下，默认电流为正
    checkBox_joint_1_possitiveCurrent->setChecked(true);
    checkBox_joint_2_possitiveCurrent->setChecked(true);
    checkBox_joint_3_possitiveCurrent->setChecked(true);
    checkBox_joint_4_possitiveCurrent->setChecked(true);
    checkBox_joint_5_possitiveCurrent->setChecked(true);
    checkBox_joint_6_possitiveCurrent->setChecked(true);
    checkBox_joint_7_possitiveCurrent->setChecked(true);

}

//modbus线程初始化
void MainWindow::modbusInit()
{
    //创建子线程对象
    QThread *thread_modbus = new QThread;
    //创建任务类对象
    MyModbus *myModbus = new MyModbus;
    //任务对象移动到子线程中
    myModbus->moveToThread(thread_modbus);

    //线程开启
    connect(thread_modbus,&QThread::started,myModbus,&MyModbus::startWork,Qt::QueuedConnection);
    thread_modbus->start();

    //    qDebug()<< "modbus thread:" << thread();

    //子线程对UI按钮使能的操作
    connect(myModbus,&MyModbus::execButtonSetSignal,this,[this](bool setEnable){
        pushButton_execute->setEnabled(setEnable);
    });
    connect(myModbus,&MyModbus::readButtonSetSignal,this,[this](bool setEnable){
        pushButton_read->setEnabled(setEnable);
    });

    //建立UI界面下参数设置与子线程参数设置的连接
    connect(this,&MainWindow::sendNumberOfRetries,myModbus,&MyModbus::getNumberOfRetries,Qt::QueuedConnection);
    connect(this,&MainWindow::sendTimeOut,myModbus,&MyModbus::getTimeOut,Qt::QueuedConnection);
    connect(this,&MainWindow::sendTcpAddress,myModbus,&MyModbus::getTcpAddress,Qt::QueuedConnection);
    connect(this,&MainWindow::sendTcpPort,myModbus,&MyModbus::getTcpPort,Qt::QueuedConnection);

    //建立UI界面按钮与myModbus类的函数的连接
    connect(pushButton_connectModbus,&QPushButton::clicked,myModbus,&MyModbus::connectModbus,Qt::QueuedConnection);
    connect(pushButton_disconnectModbus,&QPushButton::clicked,myModbus,&MyModbus::disconnectModbus,Qt::QueuedConnection);
    connect(pushButton_read,&QPushButton::clicked,myModbus,&MyModbus::readData);
    connect(pushButton_moveToP1,&QPushButton::clicked,myModbus,&MyModbus::moveToP1Clicked);
    connect(pushButton_moveToP2,&QPushButton::clicked,myModbus,&MyModbus::moveToP2Clicked);
    connect(pushButton_moveToP3,&QPushButton::clicked,myModbus,&MyModbus::moveToP3Clicked);
    connect(pushButton_autoMove,&QPushButton::clicked,myModbus,&MyModbus::autoMoveClicked);
    connect(pushButton_stopSendPoint,&QPushButton::clicked,myModbus,&MyModbus::stopSendPoint);
    connect(this,&MainWindow::sendCurrentJointsAngles,myModbus,&MyModbus::setCurrentJointsAngles,Qt::QueuedConnection);
    connect(pushButton_masterValve,&QPushButton::clicked,myModbus,[=](){
        if(pushButton_masterValve->text()==QStringLiteral("打开总阀"))
        {
            myModbus->masterValveEnable();
            pushButton_masterValve->setText(QStringLiteral("关闭总阀"));
        }
        else
        {
            myModbus->masterValveUnable();
            pushButton_masterValve->setText(QStringLiteral("打开总阀"));
        }
    });
    connect(pushButton_autoMoveLine,&QPushButton::clicked,myModbus,&MyModbus::moveToPointsSequenceClicked);
    connect(pushButton_autoMoveLineDiverse,&QPushButton::clicked,myModbus,&MyModbus::moveToPointsSequenceDiverseClicked);
    connect(pushButton_stopLine,&QPushButton::clicked,myModbus,&MyModbus::stopMoveToPointsSequence);
    //手爪关节
    connect(comboBox_gripperSpeedLevel, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index){
        myModbus->getGripperSpeed(index);
    });
    connect(pushButton_catch,&QPushButton::clicked,myModbus,&MyModbus::gripperCatchClicked);
    connect(pushButton_release,&QPushButton::clicked,myModbus,&MyModbus::gripperReleaseClicked);
    connect(pushButton_close,&QPushButton::pressed,myModbus,&MyModbus::gripperClosePressed);
    connect(pushButton_close,&QPushButton::released,myModbus,&MyModbus::gripperCloseReleased);
    connect(pushButton_open,&QPushButton::pressed,myModbus,&MyModbus::gripperOpenPressed);
    connect(pushButton_open,&QPushButton::released,myModbus,&MyModbus::gripperOpenReleased);

    //子线程发送数据到主线程
    connect(myModbus,&MyModbus::sendDataToProcess,this,&MainWindow::readDataProcess,Qt::QueuedConnection);
    connect(myModbus,&MyModbus::sendRegisterData,this,&MainWindow::readRegesiterProcess,Qt::QueuedConnection);
    connect(myModbus,&MyModbus::saveWriteDataSignal,this,&MainWindow::saveDataToWrite,Qt::QueuedConnection);

    //主线程发送数据到子线程
    connect(this,&MainWindow::sendThreePointList,myModbus,&MyModbus::getThreePointList);
    connect(this,&MainWindow::sendLinePointsList,myModbus,&MyModbus::getLinePointsList);

    COMMANDER_INTERFACE->init(myModbus);
}

void MainWindow::cameraInit()
{
    //创建子线程对象
    QThread *thread_camera = new QThread;
    //创建任务类对象
    MyCamera *myCamera = new MyCamera;
    //任务对象移动到子线程中
    myCamera->moveToThread(thread_camera);
    //开启线程
    thread_camera->start();

    //    qDebug()<< "camera thread:" << ();

    //建立UI界面按钮与myCamera类的函数的连接
    connect(pushButton_searchCamera,&QPushButton::clicked,myCamera,&MyCamera::searchDevice);
    connect(pushButton_openCamera,&QPushButton::clicked,myCamera,&MyCamera::cameraOn);
    connect(pushButton_closeCamera,&QPushButton::clicked,myCamera,&MyCamera::cameraOff);
    connect(pushButton_cameraStartGrabbing,&QPushButton::clicked,myCamera,&MyCamera::cameraStartGrabbing);
    connect(pushButton_cameraStopGrabbing,&QPushButton::clicked,myCamera,&MyCamera::cameraStopGrabbing);
    connect(pushButton_cameraSoftwareOnceTriggle,&QPushButton::clicked,myCamera,&MyCamera::cameraSoftwareOnceTriggle);
    connect(pushButton_getCameraParameter,&QPushButton::clicked,myCamera,&MyCamera::getCameraParameter);
    connect(pushButton_setCameraParameter,&QPushButton::clicked,myCamera,&MyCamera::setCameraParameter);

    connect(radioButton_cameraTriggerMode,&QRadioButton::clicked,myCamera,&MyCamera::cameraTriggerMode);
    connect(radioButton_cameraContinusMode,&QRadioButton::clicked,myCamera,&MyCamera::cameraContinusMode);

    //建立myCamera类信号与UI界面操作的连接
    connect(myCamera,&MyCamera::sendErrorMeassage,this,&MainWindow::showErrorMessage);
    connect(myCamera,&MyCamera::sendComboBoxCameraDeviceItems,this,&MainWindow::addComboBoxCameraDeviceItems);
    connect(myCamera,&MyCamera::sendTriggerModeRadioCheckedSignal,radioButton_cameraTriggerMode,&QRadioButton::setChecked);
    connect(myCamera,&MyCamera::sendContinusModeRadioCheckedSignal,radioButton_cameraContinusMode,&QRadioButton::setChecked);
    connect(myCamera,&MyCamera::sendSoftwareTriggerEnableSignal,checkBox_cameraSoftwareTriggle,&QCheckBox::setEnabled);
    connect(myCamera,&MyCamera::sendSoftwareOnceEnableSignal,pushButton_cameraSoftwareOnceTriggle,&QPushButton::setEnabled);

    connect(myCamera,&MyCamera::sendOneFrameImage,this,&MainWindow::displayOneFrame);

}

void MainWindow::controllerInit()
{
    //创建子线程对象
    QThread *thread_controller = new QThread;
    //创建任务类对象
    MyController *myController = new MyController;

    //任务对象移动到子线程中
    myController->moveToThread(thread_controller);

    thread_controller->start();

    CONTROLLER_INTERFACE->init(ICOMMANDER_INTERFACE);

    connect(pushButton_trajectoryPlan,&QPushButton::clicked,myController,[=](){
        //QString points_to_plan = three_points_list.at(0);
        QString points_to_plan = lineEdit_P1->text();
        if (points_to_plan!=NULL)
        {
            //去除换行符号
            if(points_to_plan.contains("\r"))
                points_to_plan.remove("\r");
            //关节按空格区分
            QStringList thetaList =  points_to_plan.split(" ");

            JointTheta theta;
            //double speed = 1;
            double speed = lineEdit_jointsSpeed->text().toDouble();
            speed = speed/10.0;
            theta.theta1_ = thetaList.at(0).toDouble();
            theta.theta2_ = thetaList.at(1).toDouble();
            theta.theta3_ = thetaList.at(2).toDouble();
            theta.theta4_ = thetaList.at(3).toDouble();
            theta.theta5_ = thetaList.at(4).toDouble();
            theta.theta6_ = thetaList.at(5).toDouble();

            CONTROLLER_INTERFACE->OnClickedMoveRobotJoints(theta,speed);
        }
    });
}

//对读取的数据做处理
void MainWindow::readDataProcess(bool modbusStatus,quint16 valveFlag,QModbusReply *reply)
{
    bool systemStatus;
    JointTheta jointTheta;

    if(!reply)
    {
        return;
    }
    else
    {
        QByteArray receiveData;
        QDataStream receiveStream(&receiveData,QIODevice::WriteOnly);
        receiveStream << reply->rawResult(); //写在外部函数中只能用这种形式

        //    qDebug()<<"opopopopopopopop"<<receiveData.toHex();
        //    LARGE_INTEGER nFreq;
        //    LARGE_INTEGER t1;
        //    LARGE_INTEGER t2;
        //    double dt;
        //    QueryPerformanceFrequency(&nFreq);
        //    QueryPerformanceCounter(&t1);

        static bool ok;
        //只有大小为64的数据才是一帧完整数据
        if(receiveData.size()!=64)
        {
            return;
        }
        else
        {
            /*****************************系统状态*****************************/
            switch (receiveData.mid(2,2).toHex().toInt(&ok,16)) {
            case 0:
                sysStatus->setText(QStringLiteral("正常"));
                systemStatus = TRUE;
                break;
            case 1:
                sysStatus->setText(QStringLiteral("比例阀模块错误"));
                systemStatus = FALSE;
                break;
            case 2:
                sysStatus->setText(QStringLiteral("参数错误"));
                systemStatus = FALSE;
                break;
            case 3:
                sysStatus->setText(QStringLiteral("通信错误"));
                systemStatus = FALSE;
                break;
            }
            //系统电压
            sysVoltage->setText(QString::number(receiveData.mid(4,2).toHex().toInt(&ok,16))+"mV");
            //系统电流
            sysECurrent->setText(QString::number(receiveData.mid(6,2).toHex().toInt(&ok,16))+"mA");
            //系统温度
            sysTemperature->setText(QString::number(receiveData.mid(8,2).toHex().toInt(&ok,16))+QStringLiteral("℃"));


            /*****************************关节1（肩关节）*****************************/
            //工作状态
            switch (receiveData.mid(10,2).toHex().toInt(&ok,16)) {
            case 0:
                label_joint_1_status_value->setText(QStringLiteral("正常"));
                break;
            case 1:
                label_joint_1_status_value->setText(QStringLiteral("比例阀模块错误"));
                break;
            case 2:
                label_joint_1_status_value->setText(QStringLiteral("参数错误"));
                break;
            }
            //角度传感器

            label_joint_1_status_angle_value->setText(QString::number(receiveData.mid(12,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            jointTheta.theta1_ = QString::number(receiveData.mid(12,2).toHex().toInt(&ok,16)*0.01).toDouble();
            //    qDebug()<<receiveData.mid(12,2);
            //    qDebug()<<receiveData.mid(12,2).toHex();
            //    qDebug()<<receiveData.mid(12,2).toInt();
            //    qDebug()<<receiveData.mid(12,2).toLong(&ok,10);
            //压力传感器
            //    label_joint_1_status_pressure_value->setText(QString::number(receiveData.mid(14,2).toHex().toInt(&ok,16))+"N");


            /*****************************关节2（大臂关节）*****************************/
            //工作状态
            switch (receiveData.mid(16,2).toHex().toInt(&ok,16)) {
            case 0:
                label_joint_2_status_value->setText(QStringLiteral("正常"));
                break;
            case 1:
                label_joint_2_status_value->setText(QStringLiteral("比例阀模块错误"));
                break;
            case 2:
                label_joint_2_status_value->setText(QStringLiteral("参数错误"));
                break;
            }
            //角度传感器
            label_joint_2_status_angle_value->setText(QString::number(receiveData.mid(18,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            jointTheta.theta2_ = QString::number(receiveData.mid(18,2).toHex().toInt(&ok,16)*0.01).toDouble();
            //压力传感器
            //    label_joint_2_status_pressure_value->setText(QString::number(receiveData.mid(20,2).toHex().toInt(&ok,16))+"N");


            /*****************************关节3（小臂关节）*****************************/
            //工作状态
            switch (receiveData.mid(22,2).toHex().toInt(&ok,16)) {
            case 0:
                label_joint_3_status_value->setText(QStringLiteral("正常"));
                break;
            case 1:
                label_joint_3_status_value->setText(QStringLiteral("比例阀模块错误"));
                break;
            case 2:
                label_joint_3_status_value->setText(QStringLiteral("参数错误"));
                break;
            }
            //角度传感器
            label_joint_3_status_angle_value->setText(QString::number(receiveData.mid(24,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            jointTheta.theta3_ = QString::number(receiveData.mid(24,2).toHex().toInt(&ok,16)*0.01).toDouble();
            //压力传感器
            //    label_joint_3_status_pressure_value->setText(QString::number(receiveData.mid(26,2).toHex().toInt(&ok,16))+"N");


            /*****************************关节4（肘关节）*****************************/
            //工作状态
            switch (receiveData.mid(28,2).toHex().toInt(&ok,16)) {
            case 0:
                label_joint_4_status_value->setText(QStringLiteral("正常"));
                break;
            case 1:
                label_joint_4_status_value->setText(QStringLiteral("比例阀模块错误"));
                break;
            case 2:
                label_joint_4_status_value->setText(QStringLiteral("参数错误"));
                break;
            }
            //角度传感器
            label_joint_4_status_angle_value->setText(QString::number(receiveData.mid(30,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            jointTheta.theta4_ = QString::number(receiveData.mid(30,2).toHex().toInt(&ok,16)*0.01).toDouble();
            //压力传感器
            //    label_joint_4_status_pressure_value->setText(QString::number(receiveData.mid(32,2).toHex().toInt(&ok,16))+"N");


            /*****************************关节5(手腕摆动关节)*****************************/
            //工作状态
            switch (receiveData.mid(34,2).toHex().toInt(&ok,16)) {
            case 0:
                label_joint_5_status_value->setText(QStringLiteral("正常"));
                break;
            case 1:
                label_joint_5_status_value->setText(QStringLiteral("比例阀模块错误"));
                break;
            case 2:
                label_joint_5_status_value->setText(QStringLiteral("参数错误"));
                break;
            }
            //角度传感器
            label_joint_5_status_angle_value->setText(QString::number(receiveData.mid(36,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            jointTheta.theta5_ = QString::number(receiveData.mid(36,2).toHex().toInt(&ok,16)*0.01).toDouble();
            //压力传感器
            //    label_joint_5_status_pressure_value->setText(QString::number(receiveData.mid(38,2).toHex().toInt(&ok,16))+"N");


            /*****************************关节6(手腕旋转关节)*****************************/
            //工作状态
            switch (receiveData.mid(40,2).toHex().toInt(&ok,16)) {
            case 0:
                label_joint_6_status_value->setText(QStringLiteral("正常"));
                break;
            case 1:
                label_joint_6_status_value->setText(QStringLiteral("比例阀模块错误"));
                break;
            case 2:
                label_joint_6_status_value->setText(QStringLiteral("参数错误"));
                break;
            }
            //角度传感器
            label_joint_6_status_angle_value->setText(QString::number(receiveData.mid(42,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            jointTheta.theta6_ = QString::number(receiveData.mid(42,2).toHex().toInt(&ok,16)*0.01).toDouble();
            //压力传感器
            //    label_joint_6_status_pressure_value->setText(QString::number(receiveData.mid(44,2).toHex().toInt(&ok,16))+"N");


            /*****************************关节7(手爪关节)*****************************/
            //工作状态
            switch (receiveData.mid(46,2).toHex().toInt(&ok,16)) {
            case 0:
                label_joint_7_status_value->setText(QStringLiteral("正常"));
                break;
            case 1:
                label_joint_7_status_value->setText(QStringLiteral("比例阀模块错误"));
                break;
            case 2:
                label_joint_7_status_value->setText(QStringLiteral("参数错误"));
                break;
            }
            //角度传感器
            label_joint_7_status_angle_value->setText(QString::number(receiveData.mid(48,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            //压力传感器
            //    label_joint_7_status_pressure_value->setText(QString::number(receiveData.mid(50,2).toHex().toInt(&ok,16))+"N");

            /*****************************六维力传感器******************************/
            lineEdit_Fx->setText(QString::number(INT16(receiveData.mid(52,2).toHex().toInt(&ok,16))));
            lineEdit_Fy->setText(QString::number(INT16(receiveData.mid(54,2).toHex().toInt(&ok,16))));
            lineEdit_Fz->setText(QString::number(INT16(receiveData.mid(56,2).toHex().toInt(&ok,16))));
            lineEdit_Tx->setText(QString::number(INT16(receiveData.mid(58,2).toHex().toInt(&ok,16))));
            lineEdit_Ty->setText(QString::number(INT16(receiveData.mid(60,2).toHex().toInt(&ok,16))));
            lineEdit_Tz->setText(QString::number(INT16(receiveData.mid(62,2).toHex().toInt(&ok,16))));
        }
        //    QueryPerformanceCounter(&t2);
        //    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
        //    qDebug()<<QStringLiteral("一次读数据显示处理的时间：")<<dt*1000000;

        bool sysStatusSentToPlan = modbusStatus && systemStatus && (valveFlag == 0x0001);

        saveDataRead();

        current_joints_angles[0] = jointTheta.theta1_;
        current_joints_angles[1] = jointTheta.theta2_;
        current_joints_angles[2] = jointTheta.theta3_;
        current_joints_angles[3] = jointTheta.theta4_;
        current_joints_angles[4] = jointTheta.theta5_;
        current_joints_angles[5] = jointTheta.theta6_;

        CONTROLLER_INTERFACE->updateRobotJointTheta(sysStatusSentToPlan,jointTheta);

    }
}

int flushfff = 0;
void MainWindow::readRegesiterProcess(QModbusReply *replyRegister)
{
    if(!replyRegister)
    {
        return;
    }
    else
    {
        QByteArray receiveData;
        QDataStream receiveStream(&receiveData,QIODevice::WriteOnly);
        receiveStream << replyRegister->rawResult(); //写在外部函数中只能用这种形式

        static bool ok;
        //只有大小为64的数据才是一帧完整数据
        if(receiveData.size()!=28)
        {
            return;
        }
        else
        {
            label_joint_7_status_value->setText(QString::number(receiveData.mid(2,2).toHex().toInt(&ok,16)));
            label_joint_1_status_value->setText(QString::number(receiveData.mid(4,2).toHex().toInt(&ok,16)));
            label_joint_1_status_pressure_value->setText(QString::number(receiveData.mid(6,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            label_joint_2_status_value->setText(QString::number(receiveData.mid(8,2).toHex().toInt(&ok,16)));
            label_joint_2_status_pressure_value->setText(QString::number(receiveData.mid(10,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            label_joint_3_status_value->setText(QString::number(receiveData.mid(12,2).toHex().toInt(&ok,16)));
            label_joint_3_status_pressure_value->setText(QString::number(receiveData.mid(14,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            label_joint_4_status_value->setText(QString::number(receiveData.mid(16,2).toHex().toInt(&ok,16)));
            label_joint_4_status_pressure_value->setText(QString::number(receiveData.mid(18,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            label_joint_5_status_value->setText(QString::number(receiveData.mid(20).toHex().toInt(&ok,16)));
            label_joint_5_status_pressure_value->setText(QString::number(receiveData.mid(22,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            label_joint_6_status_value->setText(QString::number(receiveData.mid(24,2).toHex().toInt(&ok,16)));
//            label_joint_6_status_pressure_value->setText(QString::number(receiveData.mid(26,2).toHex().toInt(&ok,16)*0.01)+QStringLiteral("°"));
            label_joint_6_status_pressure_value->setText(QString::number(flushfff++,10,5));
            //这里没有当前角度值，不能更新

//            JointTheta jointTheta;
//            bool sysStatusSentToPlan = false;

//            current_joints_angles[0] = jointTheta.theta1_;
//            current_joints_angles[1] = jointTheta.theta2_;
//            current_joints_angles[2] = jointTheta.theta3_;
//            current_joints_angles[3] = jointTheta.theta4_;
//            current_joints_angles[4] = jointTheta.theta5_;
//            current_joints_angles[5] = jointTheta.theta6_;

//            CONTROLLER_INTERFACE->updateRobotJointTheta(sysStatusSentToPlan,jointTheta);
        }
    }
}

void MainWindow::saveDataRead()
{
    QTime current_time =QTime::currentTime();
    //    int hour = current_time.hour();//当前的小时
    QString hourString = QString::number(current_time.hour());
    //    int minute = current_time.minute();//当前的分
    QString minuteString = QString::number(current_time.minute());
    //    int second = current_time.second();//当前的秒
    QString secondString = QString::number(current_time.second());
    //    int msec = current_time.msec();//当前的毫秒
    QString msecString = QString::number(current_time.msec());

    //    qDebug()<<hour<<":"<<minute<<":"<<second<<":"<<msec;
    QString time = hourString+":"+minuteString+":"+secondString+":"+msecString +" data read : ";

    if(!directory_data_read.isEmpty())
    {
        //存储传感器数据到文本中
        QFile file(directory_data_read);
        if(file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        {
            QTextStream out(&file);
            if(label_joint_1_status_angle_value->text() != NULL && label_joint_2_status_angle_value->text() != NULL
                    && label_joint_3_status_angle_value->text() != NULL && label_joint_4_status_angle_value->text() != NULL
                    && label_joint_5_status_angle_value->text() != NULL&& label_joint_6_status_angle_value->text() != NULL)
            {
                QString txtSaveString;
                txtSaveString = time + "\t" + label_joint_1_status_angle_value->text() + "\t"
                        + label_joint_2_status_angle_value->text() + "\t" + label_joint_3_status_angle_value->text()
                        + "\t" + label_joint_4_status_angle_value->text()+ "\t" + label_joint_5_status_angle_value->text()
                        + "\t" + label_joint_6_status_angle_value->text();
                out << txtSaveString << Qt::endl;
            }
        }
        file.close();
    }
    else
    {
        QMessageBox::information(this,tr("this is information dialog"),QStringLiteral("文件目录无法打开"));
        return;
    }
}

void MainWindow::saveDataToWrite(double *write_data)
{
    double a[6];
    memcpy(a,write_data,sizeof(double)*6);

    QTime current_time =QTime::currentTime();
    //    int hour = current_time.hour();//当前的小时
    QString hourString = QString::number(current_time.hour());
    //    int minute = current_time.minute();//当前的分
    QString minuteString = QString::number(current_time.minute());
    //    int second = current_time.second();//当前的秒
    QString secondString = QString::number(current_time.second());
    //    int msec = current_time.msec();//当前的毫秒
    QString msecString = QString::number(current_time.msec());

    //    qDebug()<<hour<<":"<<minute<<":"<<second<<":"<<msec;
    QString time = hourString+":"+minuteString+":"+secondString+":"+msecString + " data to write : ";

    if(!directory_save_trajectory.isEmpty())
    {
        //存储传感器数据到文本中
        QFile file(directory_save_trajectory);
        if(file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        {
            QTextStream out(&file);
            if(write_data!=NULL)
            {
                QString txtSaveString;
                txtSaveString = time + "\t" + QString::number(write_data[0]) + "\t" + QString::number(write_data[1],10,2) + "\t"
                        + QString::number(write_data[2],10,2) + "\t" + QString::number(write_data[3],10,2)+ "\t"
                        + QString::number(write_data[4],10,2) + "\t" + QString::number(write_data[5],10,2);
                out << txtSaveString << Qt::endl;
            }
        }
        file.close();
    }
    else
    {
        QMessageBox::information(this,tr("this is information dialog"),QStringLiteral("文件目录无法打开"));
        return;
    }
}

void MainWindow::saveDataWritten(double *write_data)
{
    double a[6];
    memcpy(a,write_data,sizeof(double)*6);

    QTime current_time =QTime::currentTime();
    //    int hour = current_time.hour();//当前的小时
    QString hourString = QString::number(current_time.hour());
    //    int minute = current_time.minute();//当前的分
    QString minuteString = QString::number(current_time.minute());
    //    int second = current_time.second();//当前的秒
    QString secondString = QString::number(current_time.second());
    //    int msec = current_time.msec();//当前的毫秒
    QString msecString = QString::number(current_time.msec());

    //    qDebug()<<hour<<":"<<minute<<":"<<second<<":"<<msec;
    QString time = hourString+":"+minuteString+":"+secondString+":"+msecString + " data written : ";

    if(!directory_save_trajectory.isEmpty())
    {
        //存储传感器数据到文本中
        QFile file(directory_save_trajectory);
        if(file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        {
            QTextStream out(&file);
            if(write_data!=NULL)
            {
                QString txtSaveString;
                txtSaveString = time + "\t" + QString::number(write_data[0]) + "\t" + QString::number(write_data[1],10,2) + "\t"
                        + QString::number(write_data[2],10,2) + "\t" + QString::number(write_data[3],10,2)+ "\t"
                        + QString::number(write_data[4],10,2) + "\t" + QString::number(write_data[5],10,2);
                out << txtSaveString << Qt::endl;
            }
        }
        file.close();
    }
    else
    {
        QMessageBox::information(this,tr("this is information dialog"),QStringLiteral("文件目录无法打开"));
        return;
    }
}

//设置存储目录
void MainWindow::on_pushButton_saveDirectory_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"open file dialog","./","TXT files(*.txt);;ALL files(*.*)");
    directory_save = fileName;
    lineEdit_fileSaveAddress->setText(directory_save);
    QFile file(directory_save);

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        QString textContent = in.readAll();
        file.close();
    }
    else
    {
        QMessageBox::information(this,tr("Information Dialog"),QStringLiteral("文件目录无法打开"));
        return;
    }
    //    QMessageBox::information(this,tr("show dirTxt"),tr(directory_save.toUtf8()));
}

//记录当前的数据
void MainWindow::on_pushButton_saveOnce_clicked()
{
    QTime current_time =QTime::currentTime();
    //    int hour = current_time.hour();//当前的小时
    QString hourString = QString::number(current_time.hour());
    //    int minute = current_time.minute();//当前的分
    QString minuteString = QString::number(current_time.minute());
    //    int second = current_time.second();//当前的秒
    QString secondString = QString::number(current_time.second());
    //    int msec = current_time.msec();//当前的毫秒
    QString msecString = QString::number(current_time.msec());

    //    qDebug()<<hour<<":"<<minute<<":"<<second<<":"<<msec;
    QString time = hourString+":"+minuteString+":"+secondString+":"+msecString;
    if(!directory_save.isEmpty())
    {
        //存储传感器数据到文本中
        QFile file(directory_save);
        if(file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        {
            QTextStream out(&file);
            if(label_joint_1_status_angle_value->text() != NULL && label_joint_2_status_angle_value->text() != NULL
                    && label_joint_3_status_angle_value->text() != NULL && label_joint_4_status_angle_value->text() != NULL
                    && label_joint_5_status_angle_value->text() != NULL&& label_joint_6_status_angle_value->text() != NULL)
            {
                QString txtSaveString;
                txtSaveString = time + "\t" + label_joint_1_status_angle_value->text() + "\t"
                        + label_joint_2_status_angle_value->text() + "\t" + label_joint_3_status_angle_value->text()
                        + "\t" + label_joint_4_status_angle_value->text()+ "\t" + label_joint_5_status_angle_value->text()
                        + "\t" + label_joint_6_status_angle_value->text();
                out << txtSaveString << Qt::endl;
            }
        }
        file.close();
    }
    else
    {
        QMessageBox::information(this,tr("this is information dialog"),QStringLiteral("文件目录无法打开"));
        return;
    }
}

//连续存储
void MainWindow::on_pushButton_saveCont_clicked()
{
    if(!timer_save->isActive())
        timer_save->start(10);
}

//停止连续存储
void MainWindow::on_pushButton_stopSave_clicked()
{
    if(timer_save->isActive())
        timer_save->stop();
}

//多点连发的目标点文件路径设置
void MainWindow::on_pushButton_getThreePointsFileDir_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"open file dialog","./","TXT files(*.txt);;ALL files(*.*)");
    directory_get_three_points = fileName;
    lineEdit_pointsDir->setText(directory_get_three_points);
}

//获取目标文件中的点
void MainWindow::on_pushButton_getThreePoints_clicked()
{
    QFile file_setPoints(directory_get_three_points);

    if(file_setPoints.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file_setPoints);
        QString textContent = in.readAll();

        three_points_list = textContent.split("\n");
        lineEdit_P1->setText(three_points_list.at(0));
        lineEdit_P2->setText(three_points_list.at(1));
        lineEdit_P3->setText(three_points_list.at(2));

        emit sendThreePointList(three_points_list);//发送点list给子线程

        file_setPoints.close();
    }
    else
    {
        QMessageBox::information(this,tr("this is information dialog"),QStringLiteral("文件目录无法打开"));
        return;
    }
}

//设置直线测试的点文件目录
void MainWindow::on_pushButton_getPointsSequenceDirectory_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"open file dialog","./","TXT files(*.txt);;ALL files(*.*)");
    directory_get_line_points = fileName;
    lineEdit_pointsListDir->setText(directory_get_line_points);
}

//从文件中获取直线测试的点序列
void MainWindow::on_pushButton_getPointsSequence_clicked()
{
    QFile file_setPointsList(directory_get_line_points);

    if(file_setPointsList.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file_setPointsList);
        QString textContent = in.readAll();
        QStringList linePoints = textContent.split("\n");
        emit sendLinePointsList(linePoints);
        QMessageBox::information(this,tr("information"),QStringLiteral("获取直线序列成功"));
        file_setPointsList.close();
    }
    else
    {
        QMessageBox::information(this,tr("information"),QStringLiteral("文件目录无法打开"));
        return;
    }
}

//生成重试次数改变的信号，并将其改变值传给子线程
void MainWindow::on_retriesSpin_valueChanged()
{
    emit sendNumberOfRetries(retriesSpin->value());
}

//生成超时改变的信号，并将其改变值传给子线程
void MainWindow::on_timeoutSpin_valueChanged()
{
    emit sendTimeOut(timeoutSpin->value());
}

//生成IP地址改变的信号，并将其改变值传给子线程
void MainWindow::on_tcpAddressEdit_textChanged()
{
    emit sendTcpAddress(tcpAddressEdit->text());
}

//生成端口号改变的信号，并将其改变值传给子线程
void MainWindow::on_tcpPortEdit_textChanged()
{
    emit sendTcpPort(tcpPortEdit->text());
}

//子线程调用的错误提示
void MainWindow::showErrorMessage(QString error_massage)
{
    QMessageBox::information(this,tr("Information Dialog"),error_massage);
}

void MainWindow::addComboBoxCameraDeviceItems(QString itemString)
{
    comboBox_cameraDevice->clear();
    comboBox_cameraDevice->addItem(itemString);
}

void MainWindow::displayOneFrame(MV_FRAME_OUT stImageInfo,MV_DISPLAY_FRAME_INFO stDisplayInfo)
{
    qDebug()<<"tag: display one frame";

    stDisplayInfo.hWnd = (HWND)label_leftCameraGraph->winId();
    stDisplayInfo.pData = stImageInfo.pBufAddr;
    stDisplayInfo.nDataLen = stImageInfo.stFrameInfo.nFrameLen;
    stDisplayInfo.nWidth = stImageInfo.stFrameInfo.nWidth;
    stDisplayInfo.nHeight = stImageInfo.stFrameInfo.nHeight;
    stDisplayInfo.enPixelType = stImageInfo.stFrameInfo.enPixelType;

    myMvCamera->DisplayOneFrame(&stDisplayInfo);
}

void MainWindow::on_pushButton_getTrajectorySaveDirectory_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"open file dialog","./","TXT files(*.txt);;ALL files(*.*)");
    directory_save_trajectory = fileName;
    lineEdit_trajectorySaveDirectory->setText(directory_save_trajectory);
}
