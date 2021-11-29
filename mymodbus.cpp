#include <QTime>
#include <QFile>
#include <QMessageBox>

#include "mymodbus.h"

MyModbus::MyModbus(QObject *parent) : QObject(parent)
{
    connect(timer_read_write_pool,&QTimer::timeout,this,&MyModbus::readAndWritePool);
    connect(timer_read_write_register_test,&QTimer::timeout,this,&MyModbus::readWriteRegisterTest);
    connect(timer_three_point_1,&QTimer::timeout,this,&MyModbus::moveToP1);
    connect(timer_three_point_2,&QTimer::timeout,this,&MyModbus::moveToP2);
    connect(timer_three_point_3,&QTimer::timeout,this,&MyModbus::moveToP3);
    connect(timer_points_sequence,&QTimer::timeout,this,&MyModbus::moveToPointsSequence);
    connect(timer_points_sequence_diverse,&QTimer::timeout,this,&MyModbus::moveToPointsSequenceDiverse);
    connect(timer_three_points,&QTimer::timeout,this,[this]()
    {
        static int i =0;

        if(i >= FLAG_THREE_POINTS_UPPER_BOUND)
            i = 0;

        switch (i % 3) {
        case 0:
            timer_three_point_1->start(TIMER_THREE_POINTS_SEND);
            timer_three_point_2->stop();
            timer_three_point_3->stop();
            break;
        case 1:
            timer_three_point_1->stop();
            timer_three_point_2->start(TIMER_THREE_POINTS_SEND);
            timer_three_point_3->stop();
            break;
        case 2:
            timer_three_point_1->stop();
            timer_three_point_2->stop();
            timer_three_point_3->start(TIMER_THREE_POINTS_SEND);
            break;
        }
        i++;
    });
}

void MyModbus::startWork()
{
    qDebug()<< "modbus thread:" << thread();
}

void MyModbus::connectModbus()
{
    modbusDevice = new ModbusTcpClient;

    qDebug()<<"connect";
    qDebug()<<QStringLiteral("plcID:")<<QThread::currentThread();  //

    if(modbusDevice->state()!=QModbusDevice::ConnectedState)
    {
        //modbusTCP
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter,tcp_address);
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter,tcp_port);
        modbusDevice->setTimeout(time_out);
        modbusDevice->setNumberOfRetries(number_retries);

        connect(modbusDevice, &QModbusDevice::errorOccurred, this, [this](QModbusDevice::Error) {
            qDebug().noquote() << QStringLiteral("Error: %1").arg(modbusDevice->errorString());
            disconnectModbus();
        }, Qt::QueuedConnection);

        //modbus
        connect(modbusDevice, &QModbusDevice::stateChanged, [=](QModbusDevice::State state) {
            switch (state) {
            case QModbusDevice::UnconnectedState://δ
                modbusStatus = FALSE;
                qDebug().noquote() << QStringLiteral("State: Entered unconnected state.");
                break;
            case QModbusDevice::ConnectingState://
                modbusStatus = FALSE;
                qDebug().noquote() << QStringLiteral("State: Entered connecting state.");
                break;
            case QModbusDevice::ConnectedState://
                modbusStatus = TRUE;
                qDebug().noquote() << QStringLiteral("State: Entered connected state.");
                break;
            case QModbusDevice::ClosingState://
                modbusStatus = FALSE;
                qDebug().noquote() << QStringLiteral("State: Entered closing state.");
                break;
            }
        });
        //豸
        modbusDevice->connectDevice();
    }
    timer_read_write_pool->start(MyModbus::TIMER_READ_WRITE_POOL);
    //timer_read_write_register_test->start(MyModbus::TIMER_READ_WRITE_POOL);
}

void MyModbus::disconnectModbus()
{
    stopAllTimers();

    modbusDevice->disconnect();//modbus
    modbusDevice->disconnectDevice();//modbus豸
    delete modbusDevice;
    modbusDevice = nullptr;
}

//ж
void MyModbus::stopAllTimers()
{

    //读写轮询定时器
    if(timer_read_write_pool->isActive())
        timer_read_write_pool->stop();
    //发送到点1定时器
    if(timer_three_point_1->isActive())
        timer_three_point_1->stop();
    //发送到点2
    if(timer_three_point_2->isActive())
        timer_three_point_2->stop();
    if(timer_three_point_3->isActive())
        timer_three_point_3->stop();
    if(timer_three_points->isActive())
        timer_three_points->stop();
    if(timer_points_sequence->isActive())
        timer_points_sequence->stop();
    if(timer_points_sequence_diverse->isActive())
        timer_points_sequence_diverse->stop();
}

//
int jishu = 0;
void MyModbus::masterValveEnable()
{
    master_valve_flag = 0x0001;
    //    if(timer_read_write_register_test->isActive())
    //        timer_read_write_register_test->stop();

    //    if(!timer_read_write_pool->isActive())
    //        timer_read_write_pool->start(TIMER_READ_WRITE_POOL);
}

//
void MyModbus::masterValveUnable()
{
    master_valve_flag = 0x0000;
    //    static int i = 0;
    //    if(jishu>0)
    //   {
    //    if(timer_read_write_pool->isActive())
    //        timer_read_write_pool->stop();

    //    if(!timer_read_write_register_test->isActive())
    //        timer_read_write_register_test->start(TIMER_READ_WRITE_POOL);
    //    }
    //    else
    //    {
    //        if(timer_read_write_register_test->isActive())
    //            timer_read_write_register_test->stop();

    //        if(!timer_read_write_pool->isActive())
    //            timer_read_write_pool->start(TIMER_READ_WRITE_POOL);

    //    }

    //    jishu++;



    //        joints_set = MyModbus::JOINTS_SET_UNABLE;
}

//
void MyModbus::readData()
{
    QByteArray pduData_read;
    QDataStream pduDataString(&pduData_read,QIODevice::WriteOnly);

    //pdu
    quint16 startAddress = READ_INSTRUCTIONS_START_ADDRESS;//***************
    //
    quint16 registerNum = 0x1F;//***************
    pduDataString << startAddress << registerNum;

    modbusDevice->valueChanged(pduData_read.size()+2);//0x06

    QModbusReply *reply_read = nullptr;
    reply_read = modbusDevice->sendRawRequest(QModbusRequest(QModbusRequest::FunctionCode(
                                                                 QModbusPdu::ReadInputRegisters),pduData_read),quint8(1));

    //replyfinished
    //If the request has not finished then the returned QModbusResponse instance is invalid.
    if (reply_read) {
        if (!reply_read->isFinished()) {
            connect(reply_read, &QModbusReply::finished, [reply_read, this]() {
                emit sendDataToProcess(modbusStatus,master_valve_flag,reply_read);
                qDebug() << "Receive: Asynchronous response PDU: " << reply_read->rawResult();
            });
        }
        else //
        {
            emit sendDataToProcess(modbusStatus,master_valve_flag,reply_read);
            qDebug() << "Receive: Synchronous response pdu: " << reply_read->rawResult() << Qt::endl;
        }
    }
}

//к
//void MainWindow::openLoopMode_exec()
//{
//    QByteArray pduData;
//    QDataStream pduDataString(&pduData,QIODevice::WriteOnly);

//    //дpdu
//    quint16 startAddress = quint16(0);//
//    quint16 registerNum = quint16(10);//д
//    quint8 byteLength = quint8(20);//д
//    pduDataString << startAddress << registerNum << byteLength;
//    //ú
//    quint16 xset=quint16(checkBox_joint_1_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    quint16 eCurrent = quint16(checkBox_joint_1_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_1_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //ú
//    xset=quint16(checkBox_joint_2_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_2_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_2_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //Сú
//    xset=quint16(checkBox_joint_3_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_3_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_3_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //ú
//    xset=quint16(checkBox_joint_4_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_4_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_Joint_4_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //ú
//    xset=quint16(checkBox_joint_5_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_5_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_5_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //ú
//    xset=quint16(checkBox_joint_6_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_6_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_6_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //ú
//    xset=quint16(checkBox_joint_7_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_7_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_7_currentValue->value()*540/692));
//    pduDataString << eCurrent;

//    qDebug()<<pduData.toHex();

////    l2Spin->setValue(pduData.size()+2);//pduData

//    QModbusReply *reply = nullptr;

////    const quint8 address = quint8(ui1Spin->value());

//    qDebug() << "Send: Sending custom PDU.";

////    reply = m_device->sendRawRequest(QModbusRequest(QModbusRequest::FunctionCode(16), pduData), address);

////    if (reply) {
////        execButton->setDisabled(true);
////        if (!reply->isFinished()) {
////            //reply->isFinished()
////            //Returns true when the reply has finished or was aborted
////            //
////            connect(reply, &QModbusReply::finished, [reply, this]() {
////                execButton->setEnabled(true);
////                qDebug() << "Receive: Asynchronous response PDU: " << reply->rawResult() << Qt::endl;
////            });
////        }
////        else //
////        {
////            execButton->setEnabled(true);
////            qDebug() << "Receive: Synchronous response pdu: " << reply->rawResult() << Qt::endl;
////        }
////    }
//}

//生成发送点的报文PDU
QByteArray MyModbus::generatePdu(QString point, int *gripperList)
{
    QStringList pointSep = point.split(" ");

    QByteArray pduData;
    QDataStream pduDataString(&pduData,QIODevice::WriteOnly);

    //дpdu
    quint16 startAddress = WRITE_INSTRUCTIONS_START_ADDRESS;//
    quint16 registerNum = quint16(15);//д,
    quint8 byteLength = quint8(30);//д

    pduDataString  << startAddress << registerNum << byteLength << master_valve_flag;

    /**********************1()ú**********************/
    pduDataString << joints_set;
    quint16 joints_value = quint16(round(pointSep.at(0).toFloat()*100));
    pduDataString << joints_value;

    /*********************2()ú**********************/
    pduDataString << joints_set;
    joints_value = quint16(round(pointSep.at(1).toFloat()*100));
    pduDataString << joints_value;

    /*********************3(С)ú**********************/
    pduDataString << joints_set;
    joints_value = quint16(round(pointSep.at(2).toFloat()*100));
    pduDataString << joints_value;

    /**********************4()ú***********************/
    pduDataString << joints_set;
    joints_value = quint16(round(pointSep.at(3).toFloat()*100));
    pduDataString << joints_value;

    /********************5()ú*********************/
    pduDataString << joints_set;
    joints_value = quint16(round(pointSep.at(4).toFloat()*100));
    pduDataString << joints_value;

    /********************6()ú*********************/
    pduDataString << joints_set;
    joints_value = quint16(round(pointSep.at(5).toFloat()*100));
    pduDataString << joints_value;

    /**********************7()ú**********************/
    quint16 xset = gripperList[0]==0? 0x0000:0x8000;//1п
    xset = gripperList[1]==0? xset:(xset|0x2000);
    pduDataString << xset;
    joints_value = quint16(gripperList[2]);
    pduDataString << joints_value;

    return pduData;

}

QByteArray MyModbus::generatePdu(quint16 master_flag,quint16 joint_set,
                                 double* position_point, int* gripperList)
{
    QByteArray pduData;
    QDataStream pduDataString(&pduData,QIODevice::WriteOnly);

    //дpdu
    quint16 startAddress = WRITE_INSTRUCTIONS_START_ADDRESS;//
    quint16 registerNum = quint16(15);//д,
    quint8 byteLength = quint8(30);//д
    //    quint16 open = 0x0001;

    pduDataString  << startAddress << registerNum << byteLength << master_flag;

    /**********************1()ú**********************/
    pduDataString << joint_set;
    quint16 joints_value = quint16(round(position_point[0]*100));
    pduDataString << joints_value;

    /*********************2()ú**********************/
    pduDataString << joint_set;
    joints_value = quint16(round(position_point[1]*100));
    pduDataString << joints_value;

    /*********************3(С)ú**********************/
    pduDataString << joint_set;
    joints_value = quint16(round(position_point[2]*100));
    pduDataString << joints_value;

    /**********************4()ú***********************/
    pduDataString << joint_set;
    joints_value = quint16(round(position_point[3]*100));
    pduDataString << joints_value;

    /********************5()ú*********************/
    pduDataString << joint_set;
    joints_value = quint16(round(position_point[4]*100));
    pduDataString << joints_value;

    /********************6()ú*********************/
    pduDataString << joint_set;
    joints_value = quint16(round(position_point[5]*100));
    pduDataString << joints_value;

    /**********************7()ú**********************/
    quint16 xset = gripperList[0]==0? 0x0000:0x8000;//1п
    xset = gripperList[1]==0? xset:(xset|0x2000);
    pduDataString << xset;
    joints_value = quint16(gripperList[2]);
    pduDataString << joints_value;

    return pduData;
}

//
void MyModbus::moveToPoint(QByteArray pduData)
{
    modbusDevice->valueChanged(pduData.size()+2);//0x4000~0x400E共14个寄存器28个字节加上

    QModbusReply *reply_move = nullptr;

    qDebug() << "Send: Sending custom PDU.";

    //        LARGE_INTEGER nFreq;
    //        LARGE_INTEGER t1;
    //        LARGE_INTEGER t2;
    //        double dt;
    //        QueryPerformanceFrequency(&nFreq);
    //        QueryPerformanceCounter(&t1);

    reply_move = modbusDevice->sendRawRequest(QModbusRequest(
                                                  QModbusRequest::FunctionCode(QModbusPdu::WriteMultipleRegisters),pduData),quint8(1));
    if (reply_move) {
        if (!reply_move->isFinished()) {
            //reply->isFinished()
            //Returns true when the reply has finished or was aborted
            connect(reply_move, &QModbusReply::finished, [reply_move, this]() {
                emit execButtonSetSignal(true);
                qDebug() << "Receive: Asynchronous response PDU: "
                         << reply_move->rawResult() << Qt::endl;
            });
        }
        else
        {
            qDebug() << "Receive: Synchronous response pdu: "
                     << reply_move->rawResult() << Qt::endl;
        }
    }
    //        QueryPerformanceCounter(&t2);
    //        dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    //        qDebug()<<QStringLiteral("")<<dt*1000000;

    bool ok;
    double dataWritten[6]={
        QString::number(pduData.mid(9,2).toHex().toInt(&ok,16)*0.01).toDouble(),
        QString::number(pduData.mid(13,2).toHex().toInt(&ok,16)*0.01).toDouble(),
        QString::number(pduData.mid(17,2).toHex().toInt(&ok,16)*0.01).toDouble(),
        QString::number(pduData.mid(21,2).toHex().toInt(&ok,16)*0.01).toDouble(),
        QString::number(pduData.mid(25,2).toHex().toInt(&ok,16)*0.01).toDouble(),
        QString::number(pduData.mid(29,2).toHex().toInt(&ok,16)*0.01).toDouble()
    };

//    emit saveWriteDataSignal(" data written : ",&dataWritten[0]);
    saveDataWritten(dataWritten);
}

//P1
void MyModbus::moveToP1()
{
    QString point_1 = three_points_string_list.at(0);
    if(point_1.contains("\r"))
        point_1.remove("\r");

    static int flag_p1;

    if(flag_p1 > MyModbus::FLAG_UPPER_BOUND_READ_WRITE_POLL)
    {
        flag_p1 = 0;
    }

    if( flag_p1 % 2 == 0)
    {
        point_1_pdu = generatePdu(point_1,gripper_set);
        moveToPoint(point_1_pdu);
    }
    else{
        readData();
    }
    flag_p1++;
}

void MyModbus::moveToP1Clicked()
{
    masterValveUnable();
    if(!timer_three_point_1->isActive())
        timer_three_point_1->start(TIMER_THREE_POINTS_SEND);
    if(timer_three_point_2->isActive())
        timer_three_point_2->stop();
    if(timer_three_point_3->isActive())
        timer_three_point_3->stop();
}

//P2
void MyModbus::moveToP2()
{
    QString point_2 = three_points_string_list.at(1);
    if(point_2.contains("\r"))
        point_2.remove("\r");

    static int flag_p2;

    if(flag_p2 > MyModbus::FLAG_UPPER_BOUND_READ_WRITE_POLL)
    {
        flag_p2 = 0;
    }

    if( flag_p2 % 2 == 0)
    {
        point_2_pdu = generatePdu(point_2,gripper_set);
        moveToPoint(point_2_pdu);
    }
    else{
        readData();
    }
    flag_p2++;
}

void MyModbus::moveToP2Clicked()
{
    masterValveUnable();
    if(timer_three_point_1->isActive())
        timer_three_point_1->stop();
    if(!timer_three_point_2->isActive())
        timer_three_point_2->start(TIMER_THREE_POINTS_SEND);
    if(timer_three_point_3->isActive())
        timer_three_point_3->stop();
}

//P3
void MyModbus::moveToP3()
{
    QString point_3 = three_points_string_list.at(2);
    if(point_3.contains("\r"))
        point_3.remove("\r");

    static int flag_p3;
    //
    if(flag_p3 > MyModbus::FLAG_UPPER_BOUND_READ_WRITE_POLL)
    {
        flag_p3 = 0;
    }

    if( flag_p3 % 2 == 0)
    {
        point_3_pdu = generatePdu(point_3,gripper_set);
        moveToPoint(point_3_pdu);
    }
    else{
        readData();
    }
    flag_p3++;
}

void MyModbus::moveToP3Clicked()
{
    masterValveUnable();
    if(timer_three_point_1->isActive())
        timer_three_point_1->stop();
    if(timer_three_point_2->isActive())
        timer_three_point_2->stop();
    if(!timer_three_point_3->isActive())
        timer_three_point_3->start(TIMER_THREE_POINTS_SEND);
}

void MyModbus::autoMoveClicked()
{
    if(!timer_three_points->isActive())
        timer_three_points->start(TIMER_THREE_POINTS_SWITCH);
}

void MyModbus::stopSendPoint()
{
    stopAllTimers();
}

//QStringList
void MyModbus::getThreePointList(QStringList pointList)
{
    three_points_string_list = pointList;
}

void MyModbus::getLinePointsList(QStringList list)
{
    line_points = list;
}

void MyModbus::moveToPointsSequence()
{
    //    static int lineFlag = 0;
    if(line_points_index >= 2*(line_points.size()))
        line_points_index = 0;
    if(line_points_index < 2*(line_points.size()))
    {
        if(line_flag >= MyModbus::FLAG_UPPER_BOUND_READ_WRITE_POLL)
            line_flag = 0;

        if(line_flag%2 == 0)
        {
            QString linePoint = line_points.at(floor(line_points_index/2));
            if(linePoint.contains("\r"))
                linePoint.remove("\r");
            QByteArray linePointPdu = generatePdu(linePoint,gripper_set);
            moveToPoint(linePointPdu);
        }
        else
        {
            readData();
        }
        line_flag++;
        line_points_index++;
    }
}

void MyModbus::moveToPointsSequenceClicked()
{
    line_points_index = 0;

    if(!timer_points_sequence->isActive())
        timer_points_sequence->start(TIMER_POINTS_SEQUENCE_SEND);

    if(timer_points_sequence_diverse->isActive())
        timer_points_sequence_diverse->stop();
}

void MyModbus::moveToPointsSequenceDiverse()
{

    if(line_points_index > 0)
    {
        if(line_flag_diverse % 2 == 0)
        {
            QString linePoint = line_points.at(floor(line_points_index/2));
            if(linePoint.contains("\r"))
                linePoint.remove("\r");
            QByteArray linePointPdu = generatePdu(linePoint,gripper_set);
            moveToPoint(linePointPdu);
        }
        else
        {
            readData();
        }
        line_flag_diverse++;
        line_points_index--;
    }
    else
    {
        if(line_flag_diverse % 2 == 0)
        {
            QString linePoint = line_points.at(0);
            if(linePoint.contains("\r"))
                linePoint.remove("\r");
            QByteArray linePointPdu = generatePdu(linePoint,gripper_set);
            moveToPoint(linePointPdu);
        }
        else
        {
            readData();
        }
        line_flag_diverse++;
    }
}

void MyModbus::moveToPointsSequenceDiverseClicked()
{
    line_points_index_diverse =  line_points.size()-1;

    if(timer_points_sequence->isActive())
        timer_points_sequence->stop();

    if(!timer_points_sequence_diverse->isActive())
        timer_points_sequence_diverse->start(TIMER_POINTS_SEQUENCE_SEND);
}

void MyModbus::stopMoveToPointsSequence()
{
    if(timer_points_sequence->isActive())
        timer_points_sequence->stop();
    if(timer_points_sequence_diverse->isActive())
        timer_points_sequence_diverse->stop();
    line_flag = 0;
}

void MyModbus::getGripperSpeed(int speedLevel)
{
    gripper_speed_level = speedLevel+1;
    //    qDebug()<<gripperSpeedLevel;
    switch (gripper_speed_level) {
    case 0:
        gripper_set[2] = 291;
    case 1:
        gripper_set[2] = 391;
    case 2:
        gripper_set[2] = 491;
    case 3:
        gripper_set[2] = 591;
    case 4:
        gripper_set[2] = 691;
    }
}

void MyModbus::gripperCatchClicked()
{
    gripper_set[0] = 1;//
    gripper_set[1] = 0;//
}

void MyModbus::gripperReleaseClicked()
{
    gripper_set[0] = 1;//
    gripper_set[1] = 1;//
}

void MyModbus::gripperClosePressed()
{
    gripper_set[0] = 1;//
    gripper_set[1] = 0;//
}

void MyModbus::gripperCloseReleased()
{
    gripper_set[0] = 0;//
}

void MyModbus::gripperOpenPressed()
{
    gripper_set[0] = 1;//
    gripper_set[1] = 1;//
}

void MyModbus::gripperOpenReleased()
{
    gripper_set[0] = 0;//
}

void MyModbus::setCurrentJointsAngles(double *angles)
{
    memcpy(current_joints_angles,angles,sizeof(double)*6);
}

void MyModbus::getTcpAddress(QString address)
{
    tcp_address = address;
}

void MyModbus::getTcpPort(QString port)
{
    tcp_port = port;
}

void MyModbus::getNumberOfRetries(int retries)
{
    number_retries = retries;
}

void MyModbus::getTimeOut(int out)
{
    time_out = out;
}

void MyModbus::readAndWritePool()
{
    static int flag_read_write_pool = 0;

    if(flag_read_write_pool > MyModbus::FLAG_UPPER_BOUND_READ_WRITE_POLL)
    {
        flag_read_write_pool = 0;
    }

    if( flag_read_write_pool % 2 == 0)
    {
        readData();
    }
    else
    {
        //if((master_valve_flag==MASTER_VALVE_OPEN) == system_server)
        //{
        if(master_valve_flag)
        {

            master_valve_flag = MyModbus::MASTER_VALVE_OPEN;
            joints_set = MyModbus::JOINTS_SET_ENABLE_CLOSED_LOOP;
        }
        else
        {
            master_valve_flag = MyModbus::MASTER_VALVE_CLOSE;
            joints_set = MyModbus::JOINTS_SET_UNABLE;
        }
        // }

        QByteArray trajectory_planed_pdu = generatePdu(MyModbus::master_valve_flag,MyModbus::joints_set,
                                                       MyModbus::trajectory_planed,MyModbus::gripper_set);
        moveToPoint(trajectory_planed_pdu);
    }

    flag_read_write_pool++;
}

void MyModbus::readWriteRegisterTest()
{
    static int flag_read_write_pool = 0;

    if(flag_read_write_pool > MyModbus::FLAG_UPPER_BOUND_READ_WRITE_POLL)
    {
        flag_read_write_pool = 0;
    }

    if( flag_read_write_pool % 2 == 0)
    {
        readRegister();
    }
    else
    {
        // if((master_valve_flag) /*== system_server*/)
        // {
        if(master_valve_flag/*system_server*/)
        {

            master_valve_flag = MyModbus::MASTER_VALVE_OPEN;
            joints_set = MyModbus::JOINTS_SET_ENABLE_CLOSED_LOOP;
        }
        else
        {
            master_valve_flag = MyModbus::MASTER_VALVE_CLOSE;
            joints_set = MyModbus::JOINTS_SET_UNABLE;
        }
        // }
        QByteArray trajectory_planed_pdu = generatePdu(MyModbus::master_valve_flag,MyModbus::joints_set,
                                                       MyModbus::trajectory_planed,MyModbus::gripper_set);
        moveToPoint(trajectory_planed_pdu);
    }

    flag_read_write_pool++;
}

void MyModbus::readRegister()
{
    QByteArray pduData_read;
    QDataStream pduDataString(&pduData_read,QIODevice::WriteOnly);

    //pdu
    quint16 startAddress = WRITE_INSTRUCTIONS_START_ADDRESS;//***************
    //
    quint16 registerNum = 0x0D;//***************
    pduDataString << startAddress << registerNum;

    modbusDevice->valueChanged(pduData_read.size()+2);//0x06

    QModbusReply *reply_read = nullptr;
    reply_read = modbusDevice->sendRawRequest(QModbusRequest(QModbusRequest::FunctionCode(
                                                                 QModbusPdu::ReadHoldingRegisters),pduData_read),quint8(1));

    //replyfinished
    //If the request has not finished then the returned QModbusResponse instance is invalid.
    if (reply_read) {
        if (!reply_read->isFinished()) {
            connect(reply_read, &QModbusReply::finished, [reply_read, this]() {
                emit sendRegisterData(reply_read);
                qDebug() << "Receive: Asynchronous response PDU: " << reply_read->rawResult();
            });
        }
        else //
        {
            emit sendRegisterData(reply_read);
            qDebug() << "Receive: Synchronous response pdu: " << reply_read->rawResult() << Qt::endl;
        }
    }
}

void MyModbus::saveDataToWrite(double *data_to_write)
{
    double a[6];
    memcpy(a,data_to_write,sizeof(double)*6);

    QTime current_time =QTime::currentTime();
    QString hourString = QString::number(current_time.hour());
    QString minuteString = QString::number(current_time.minute());
    QString secondString = QString::number(current_time.second());
    QString msecString = QString::number(current_time.msec());

    QString time = hourString+":"+minuteString+":"+secondString+":"+msecString + " data to write : ";

    if(!directpry_data_to_write.isEmpty())
    {
        //存储传感器数据到文本中
        QFile file(directpry_data_to_write);
        if(file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        {
            QTextStream out(&file);
            if(data_to_write!=NULL)
            {
                QString txtSaveString;
                txtSaveString = time + "\t" + QString::number(data_to_write[0]) + "\t" + QString::number(data_to_write[1],10,2) + "\t"
                        + QString::number(data_to_write[2],10,2) + "\t" + QString::number(data_to_write[3],10,2)+ "\t"
                        + QString::number(data_to_write[4],10,2) + "\t" + QString::number(data_to_write[5],10,2);
                out << txtSaveString << Qt::endl;
            }
        }
        file.close();
    }
    else
    {
//        QMessageBox::information(this,tr("this is information dialog"),QStringLiteral("文件目录无法打开"));
        return;
    }
}

void MyModbus::saveDataWritten(double *data_written)
{
    double a[6];
    memcpy(a,data_written,sizeof(double)*6);

    QTime current_time =QTime::currentTime();
    QString hourString = QString::number(current_time.hour());
    QString minuteString = QString::number(current_time.minute());
    QString secondString = QString::number(current_time.second());
    QString msecString = QString::number(current_time.msec());

    QString time = hourString+":"+minuteString+":"+secondString+":"+msecString + " data written : ";

    if(!directory_data_written.isEmpty())
    {
        //存储传感器数据到文本中
        QFile file(directory_data_written);
        if(file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
        {
            QTextStream out(&file);
            if(data_written!=NULL)
            {
                QString txtSaveString;
                txtSaveString = time + "\t" + QString::number(data_written[0]) + "\t" + QString::number(data_written[1],10,2) + "\t"
                        + QString::number(data_written[2],10,2) + "\t" + QString::number(data_written[3],10,2)+ "\t"
                        + QString::number(data_written[4],10,2) + "\t" + QString::number(data_written[5],10,2);
                out << txtSaveString << Qt::endl;
            }
        }
        file.close();
    }
    else
    {
//        QMessageBox::information(this,tr("this is information dialog"),QStringLiteral("文件目录无法打开"));
        return;
    }
}
