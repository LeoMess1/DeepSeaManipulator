#ifndef MYMODBUS_H
#define MYMODBUS_H

#include <QObject>
#include <QModbusTcpClient>
#include <QThread>
#include <QTimer>

#include "modbustcpclient.h"
#include "modbustcpclient_p.h"

class MyModbus : public QObject
{
    Q_OBJECT
public:
    explicit MyModbus(QObject *parent = nullptr);

signals:
    /******************子线程对主线程UI进行操作的信号***********************/
    void execButtonSetSignal(bool);       //执行按钮使能信号
    void readButtonSetSignal(bool);       //读取按钮使能信号

    /******************发送从UI获取Mbap的信号***********************/
    void getTi1SpinValue(int);
    void getTi2SpinValue(int);
    void getPi1SpinValue(int);
    void getPi2SpinValue(int);
    void getL1SpinValue(int);
    void getL2SpinValue(int);
    void getUi1SpinValue(int);
    void setL2SpinValue(int);
    void sendDataToProcess(QModbusReply *reply);//发送反馈的数据给主线程处理


public slots:
    /**********************TCP相关参数**************************/
    void getTcpAddress(QString);            //获取IP
    void getTcpPort(QString);               //获取端口号
    void getNumberOfRetries(int);           //获取重试次数
    void getTimeOut(int);                   //获取超时

    /******************Modbus连接***********************/
    void startWork();                       //开启线程
    void connectModbus();                   //连接modbus
    void disconnectModbus();                //断开modbus连接
    void stopAllTimers();

    /**********************功能************************/
    void MasterValve();                     //发送总阀使能的实现
    void masterValveEnable();               //开始对总阀使能的发送
    void masterValveUnable();               //停止对总阀使能的发送
    void readData();                        //读取数据的实现
    void getThreePointList(QStringList);    //从主线程获取三点连发的QStringList
    void getLinePointsList(QStringList);
    QByteArray generatePdu(QString, int[]); //生成每一帧数据的PDU部分
    void moveToPoint(QByteArray);           //位置控制实现（给定点）
    void moveToP1Clicked();
    void moveToP1();                        //发送P1点的实现
    void moveToP2Clicked();
    void moveToP2();                        //发送P2点的实现
    void moveToP3Clicked();
    void moveToP3();                        //发送P3点的实现
    void autoMoveClicked();
    void stopSendPoint();
    void moveToPointsSequence();
    void moveToPointsSequenceClicked();
    void moveToPointsSequenceDiverse();
    void moveToPointsSequenceDiverseClicked();
    void stopMoveToPointsSequence();

    /**********************手爪************************/
    void getGripperSpeed(int);              //手爪速度
    void gripperCatchClicked();             //开关模式下手爪关
    void gripperReleaseClicked();           //开关模式下手爪开
    void gripperClosePressed();             //位置模式下手爪关
    void gripperCloseReleased();            //位置模式下手爪关
    void gripperOpenPressed();              //位置模式下手爪开
    void gripperOpenReleased();             //位置模式下手爪开

private:

    /************************定时器常量*****************************/
    const int     TIMER_MASTER_VALVE                = 50;
    const int     TIMER_READ                        = 25;
    const int     TIMER_THREE_POINTS_SEND           = 25;
    const int     TIMER_THREE_POINTS_SWITCH         = 5000;
    const int     TIMER_POINTS_SEQUENCE_SEND        = 25;
    /************************标志常量*****************************/
    const int     FLAG_THREE_POINTS_UPPER_BOUND     = 3333;
    const int     FLAG_UPPER_BOUND_READ_WRITE_POLL  = 32596;//读写指令轮询发送的切换标志的上界
    /************************报文常量*****************************/
    const quint16 READ_INSTRUCTIONS_START_ADDRESS   = 0x3000;
    const quint16 WRITE_INSTRUCTIONS_START_ADDRESS  = 0x4000;
    const quint16 JOINTS_SET_ENABLE_CLOSED_LOOP     = 0xc000;
    //    const quint16 JOINTS_SET_UNABLE;



    /**************************MODBUS*****************************/
    ModbusTcpClient    *modbusDevice;

    /************************TCP相关参数变量***********************/
    int                time_out                         = 100;
    int                number_retries                   = 0;
    QString            tcp_address                      = QStringLiteral("192.168.0.240");
//    QString            tcp_address                      = QStringLiteral("127.0.0.1");
    QString            tcp_port                         = QStringLiteral("502");

    /**************************相关计时器***************************/
    QTimer            *timer_master_valve               = new QTimer(this);//单独发送总阀开
    QTimer            *timer_read                       = new QTimer(this);//只发送读指令
    QTimer            *timer_three_point_1              = new QTimer(this);//发送到指定点P1
    QTimer            *timer_three_point_2              = new QTimer(this);//发送到指定点P2
    QTimer            *timer_three_point_3              = new QTimer(this);//发送到指定点P3
    QTimer            *timer_three_points               = new QTimer(this);//循环发送三个点
    QTimer            *timer_points_sequence            = new QTimer(this);//发送点序列
    QTimer            *timer_points_sequence_diverse    = new QTimer(this);//逆序发送点序列

    QStringList       three_points_string_list;
    QStringList       line_points;

    QByteArray        point_1_pdu;
    QByteArray        point_2_pdu;
    QByteArray        point_3_pdu;

    int               line_points_index                 = 0;
    int               line_points_index_diverse         = 0;
    int               line_flag                         = 0;
    int               line_flag_diverse                 = 0;
    int               gripper_speed_level               = 2;
    int               gripper_set[3]                    = {0,0,391};
};

#endif // MYMODBUS_H
