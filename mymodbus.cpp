#include "mymodbus.h"

MyModbus::MyModbus(QObject *parent) : QObject(parent)
{
    connect(timer_master_valve,&QTimer::timeout,this,&MyModbus::MasterValve);
    connect(timer_read,&QTimer::timeout,this,&MyModbus::readData);
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
    qDebug()<<QStringLiteral("��ʾplc�߳�ID:")<<QThread::currentThread();  //��ʾ��ǰ�̵߳���ֵ

    if(modbusDevice->state()!=QModbusDevice::ConnectedState)
    {
        //����modbusTCP�ͻ�����ز���
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter,tcp_address);
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter,tcp_port);
        modbusDevice->setTimeout(time_out);
        modbusDevice->setNumberOfRetries(number_retries);

        connect(modbusDevice, &QModbusDevice::errorOccurred, this, [this](QModbusDevice::Error) {
            qDebug().noquote() << QStringLiteral("Error: %1").arg(modbusDevice->errorString());
            disconnectModbus();
        }, Qt::QueuedConnection);

        //modbus����״̬�ı�Ĵ�������
        connect(modbusDevice, &QModbusDevice::stateChanged, [](QModbusDevice::State state) {
            switch (state) {
            case QModbusDevice::UnconnectedState://δ����
                qDebug().noquote() << QStringLiteral("State: Entered unconnected state.");
                break;
            case QModbusDevice::ConnectingState://��������
                qDebug().noquote() << QStringLiteral("State: Entered connecting state.");
                break;
            case QModbusDevice::ConnectedState://������
                qDebug().noquote() << QStringLiteral("State: Entered connected state.");
                break;
            case QModbusDevice::ClosingState://�ر�
                qDebug().noquote() << QStringLiteral("State: Entered closing state.");
                break;
            }
        });
        //�����豸
        modbusDevice->connectDevice();
    }
    timer_read->start(TIMER_READ);//�����Ͼ��Զ���ȡ
}

void MyModbus::disconnectModbus()
{
    //ֹͣ��ȡ��ʱ��
    if(timer_read->isActive())
        timer_read->stop();

    modbusDevice->disconnect();//�Ͽ��뵱ǰmodbus����������ź�����
    modbusDevice->disconnectDevice();//�Ͽ���modbus�豸������
    delete modbusDevice;
    modbusDevice = nullptr;
}

//ֹͣ���ж�ʱ��
void MyModbus::stopAllTimers()
{
    if(timer_master_valve->isActive())
        timer_master_valve->stop();
    if(timer_read->isActive())
        timer_read->stop();
    if(timer_three_point_1->isActive())
        timer_three_point_1->stop();
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

//�����ܷ�ʹ�ܵ�ʵ��
void MyModbus::MasterValve()
{
    QByteArray pduData;
    QDataStream pduDataString(&pduData,QIODevice::WriteOnly);

    //ʹ������������ʽ����д���pdu
    quint16 startAddress = WRITE_INSTRUCTIONS_START_ADDRESS;//��ʼ��ַ
    quint16 registerNum = quint16(13);//д��Ĵ�������,�����٣����ܶ�
    quint8 byteLength = quint8(26);//д���ֽ���
    quint16 open = 0x0001;//�ܷ�ʹ��

    pduDataString  << startAddress << registerNum << byteLength << open;

    quint32 jointSet = 0x80000000;//���ؽ�ʹ�ܹر�

    pduDataString << jointSet<< jointSet<< jointSet<< jointSet<< jointSet<< jointSet;

    modbusDevice->valueChanged(pduData.size()+2);//33

    QModbusReply *reply_move = nullptr;//д��������Ӧ

    qDebug() << "Send: Sending custom PDU.";

    reply_move = modbusDevice->sendRawRequest(QModbusRequest(QModbusRequest::FunctionCode(16), pduData), quint8(1));
    if (reply_move) {
        //        emit execButtonSetSignal(false);
        if (!reply_move->isFinished()) {
            connect(reply_move, &QModbusReply::finished, [reply_move]() {
                qDebug() << "Receive: Asynchronous response PDU: " << reply_move->rawResult() << Qt::endl;
            });
        }
        else
        {
            qDebug() << "Receive: Synchronous response pdu: " << reply_move->rawResult() << Qt::endl;
        }
    }
}

//��ʼ���ܷ�ʹ�ܵķ���
void MyModbus::masterValveEnable()
{
    //    MasterValve();
    if(!timer_master_valve->isActive())
        timer_master_valve->start(MyModbus::TIMER_MASTER_VALVE);
}

//ֹͣ���ܷ�ʹ�ܵķ���
void MyModbus::masterValveUnable()
{
    if(timer_master_valve->isActive())
        timer_master_valve->stop();
}

//��ȡ����������
void MyModbus::readData()
{
    QByteArray pduData_read;
    QDataStream pduDataString(&pduData_read,QIODevice::WriteOnly);

    //ʹ������������ʽ����pdu
    quint16 startAddress = READ_INSTRUCTIONS_START_ADDRESS;//***************��ʼ��ַ
    //�����ά�������������ݲ���
    quint16 registerNum = 0x1F;//***************�Ĵ�������
    pduDataString << startAddress << registerNum;

    modbusDevice->valueChanged(pduData_read.size()+2);//0x06

    //��ʱ��ʼ
    //    LARGE_INTEGER nFreq;
    //    LARGE_INTEGER t1;
    //    LARGE_INTEGER t2;
    //    double dt;
    //    QueryPerformanceFrequency(&nFreq);
    //    QueryPerformanceCounter(&t1);

    //����
    QModbusReply *reply_read = nullptr;
    reply_read = modbusDevice->sendRawRequest(QModbusRequest(
             QModbusRequest::FunctionCode(QModbusPdu::ReadInputRegisters), pduData_read),quint8(1));

    //reply������ִ����finished֮��Ż��з�������
    //If the request has not finished then the returned QModbusResponse instance is invalid.
    if (reply_read) {

        //���ڴ˴�Ƶ���л���ť״̬
        //        emit execButtonSetSignal(false);
        //        emit readButtonSetSignal(false);

        if (!reply_read->isFinished()) {
            //reply->isFinished()
            //Returns true when the reply has finished or was aborted
            //�첽��Ӧ
            //������reply�����������
            connect(reply_read, &QModbusReply::finished, [reply_read, this]() {
                //���ڴ˴�Ƶ���л���ť״̬
                //                emit execButtonSetSignal(true);
                //                emit readButtonSetSignal(true);

                //�����ݵĴ�����ʾ����һ���߳�ȥ��
//                QByteArray receiveData;
//                QDataStream receiveStream(&receiveData,QIODevice::WriteOnly);
                //                receiveStream << reply_read->rawResult(); //д���ⲿ������ֻ����������ʽ
                //                emit sendDataToProcess(receiveData);
                emit sendDataToProcess(reply_read);
                qDebug() << "Receive: Asynchronous response PDU: " << reply_read->rawResult();
            });
        }
        else //ͬ����Ӧ
        {
            //�����ݵĴ�����ʾ����һ���߳�ȥ��
            //            QByteArray receiveData;
            //            QDataStream receiveStream(&receiveData,QIODevice::WriteOnly);
            //            receiveStream << reply_read->rawResult(); //д���ⲿ������ֻ����������ʽ
            emit sendDataToProcess(reply_read);
            qDebug() << "Receive: Synchronous response pdu: " << reply_read->rawResult() << Qt::endl;
        }
    }

    //    ��ʱ����
    //    QueryPerformanceCounter(&t2);
    //    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    //    qDebug() << QStringLiteral("һ�ζ������շ�ʱ�䣺") << dt*1000000 << Qt::endl;
}

//����ģʽ�µ�ִ�к���
//void MainWindow::openLoopMode_exec()
//{
//    QByteArray pduData;
//    QDataStream pduDataString(&pduData,QIODevice::WriteOnly);

//    //ʹ������������ʽ����д���pdu
//    quint16 startAddress = quint16(0);//��ʼ��ַ
//    quint16 registerNum = quint16(10);//д��Ĵ�������
//    quint8 byteLength = quint8(20);//д���ֽ���
//    pduDataString << startAddress << registerNum << byteLength;
//    //��ؽ����ú�����ֵ
//    quint16 xset=quint16(checkBox_joint_1_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    quint16 eCurrent = quint16(checkBox_joint_1_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_1_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //��۹ؽ����ú�����ֵ
//    xset=quint16(checkBox_joint_2_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_2_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_2_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //С�۹ؽ����ú�����ֵ
//    xset=quint16(checkBox_joint_3_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_3_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_3_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //��ؽ����ú�����ֵ
//    xset=quint16(checkBox_joint_4_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_4_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_Joint_4_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //����ڶ��ؽ����ú�����ֵ
//    xset=quint16(checkBox_joint_5_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_5_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_5_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //������ת�ؽ����ú�����ֵ
//    xset=quint16(checkBox_joint_6_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_6_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_6_currentValue->value()*540/692));
//    pduDataString << eCurrent;
//    //��צ�ؽ����ú�����ֵ
//    xset=quint16(checkBox_joint_7_enable->isChecked()? 1:0);
//    pduDataString << xset;
//    eCurrent = quint16(checkBox_joint_7_possitiveCurrent->isChecked()? 0:1 << 15) | quint16(round(doubleSpinBox_joint_7_currentValue->value()*540/692));
//    pduDataString << eCurrent;

//    qDebug()<<pduData.toHex();

////    l2Spin->setValue(pduData.size()+2);//�����pduData�ǲ���������������ݲ��֣�������Ҫ���Ϲ����볤�Ⱥ͵�Ԫ��ʶ������

//    QModbusReply *reply = nullptr;

////    const quint8 address = quint8(ui1Spin->value());

//    qDebug() << "Send: Sending custom PDU.";

////    reply = m_device->sendRawRequest(QModbusRequest(QModbusRequest::FunctionCode(16), pduData), address);

////    if (reply) {
////        execButton->setDisabled(true);
////        if (!reply->isFinished()) {
////            //reply->isFinished()
////            //Returns true when the reply has finished or was aborted
////            //�첽��Ӧ
////            connect(reply, &QModbusReply::finished, [reply, this]() {
////                execButton->setEnabled(true);
////                qDebug() << "Receive: Asynchronous response PDU: " << reply->rawResult() << Qt::endl;
////            });
////        }
////        else //ͬ����Ӧ
////        {
////            execButton->setEnabled(true);
////            qDebug() << "Receive: Synchronous response pdu: " << reply->rawResult() << Qt::endl;
////        }
////    }
//}

//�ջ�ģʽ������ÿһ֡���ݵ�PDU����
QByteArray MyModbus::generatePdu(QString point, int *gripperList)
{
    QStringList pointSep = point.split(" ");

    QByteArray pduData;
    QDataStream pduDataString(&pduData,QIODevice::WriteOnly);

    //ʹ������������ʽ����д���pdu
    quint16 startAddress = WRITE_INSTRUCTIONS_START_ADDRESS;//��ʼ��ַ
    quint16 registerNum = quint16(15);//д��Ĵ�������,�����٣����ܶ�
    quint8 byteLength = quint8(30);//д���ֽ���
    quint16 open = 0x0001;

    pduDataString  << startAddress << registerNum << byteLength << open;

    /**********************�ؽ�1(��ؽ�)���ú�����ֵ**********************/
    pduDataString << JOINTS_SET_ENABLE_CLOSED_LOOP;
    quint16 joints_value = quint16(round(pointSep.at(0).toFloat()*100));
    pduDataString << joints_value;

    /*********************�ؽ�2(��۹ؽ�)���ú�����ֵ**********************/
    pduDataString << JOINTS_SET_ENABLE_CLOSED_LOOP;
    joints_value = quint16(round(pointSep.at(1).toFloat()*100));
    pduDataString << joints_value;

    /*********************�ؽ�3(С�۹ؽ�)���ú�����ֵ**********************/
    pduDataString << JOINTS_SET_ENABLE_CLOSED_LOOP;
    joints_value = quint16(round(pointSep.at(2).toFloat()*100));
    pduDataString << joints_value;

    /**********************�ؽ�4(��ؽ�)���ú�����ֵ***********************/
    pduDataString << JOINTS_SET_ENABLE_CLOSED_LOOP;
    joints_value = quint16(round(pointSep.at(3).toFloat()*100));
    pduDataString << joints_value;

    /********************�ؽ�5(����ڶ��ؽ�)���ú�����ֵ*********************/
    pduDataString << JOINTS_SET_ENABLE_CLOSED_LOOP;
    joints_value = quint16(round(pointSep.at(4).toFloat()*100));
    pduDataString << joints_value;

    /********************�ؽ�6(������ת�ؽ�)���ú�����ֵ*********************/
    pduDataString << JOINTS_SET_ENABLE_CLOSED_LOOP;
    joints_value = quint16(round(pointSep.at(5).toFloat()*100));
    pduDataString << joints_value;

    /**********************�ؽ�7(��צ�ؽ�)���ú�����ֵ**********************/
    quint16 xset = gripperList[0]==0? 0x0000:0x8000;//1Ϊʹ�ܣ�ģʽֻ�п���
    xset = gripperList[1]==0? xset:(xset|0x2000);
    pduDataString << xset;
    joints_value = quint16(gripperList[2]);
    pduDataString << joints_value;

    return pduData;

}

//�ջ�ģʽ���ƶ���ָ����
void MyModbus::moveToPoint(QByteArray pduData)
{

    modbusDevice->valueChanged(pduData.size()+2);//33

    QModbusReply *reply_move = nullptr;//д��������Ӧ

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
        //        emit execButtonSetSignal(false);
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
            //            emit execButtonSetSignal(true);
            qDebug() << "Receive: Synchronous response pdu: "
                     << reply_move->rawResult() << Qt::endl;
        }
    }
    //        QueryPerformanceCounter(&t2);
    //        dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    //        qDebug()<<QStringLiteral("һ�������շ�ʱ�䣺")<<dt*1000000;
}

//����P1���ʵ��
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

//����P2���ʵ��
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

//����P3���ʵ��
void MyModbus::moveToP3()
{
    QString point_3 = three_points_string_list.at(2);
    if(point_3.contains("\r"))
        point_3.remove("\r");

    static int flag_p3;
    //Խ������
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
    if(timer_read->isActive())
        timer_read->stop();
    if(timer_three_point_1->isActive())
        timer_three_point_1->stop();
    if(timer_three_point_2->isActive())
        timer_three_point_2->stop();
    if(timer_three_point_3->isActive())
        timer_three_point_3->stop();
    if(timer_read->isActive())
        timer_read->stop();
    if(timer_three_points->isActive())
        timer_three_points->stop();
    masterValveEnable();
    qDebug()<<"Stop";
}

//�����̻߳�ȡ����������QStringList
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
    if(timer_master_valve->isActive())
        timer_master_valve->stop();

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
    //    else
    //    {
    //        if(lineFlag%2 == 0)
    //        {
    //            QString linePoint = linePoints.at(floor(linePointsIndex/2)-1);
    //            if(linePoint.contains("\r"))
    //                linePoint.remove("\r");
    //            QByteArray linePointPdu = generate_pointPdu(linePoint,gripperSet);
    //            move_to_point(linePointPdu);
    //        }
    //        else
    //        {
    //            readData();
    //        }
    //        lineFlag++;
    //    }
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
    if(timer_master_valve->isActive())
        timer_master_valve->stop();

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
    gripper_set[0] = 1;//ʹ��
    gripper_set[1] = 0;//�պ�
}

void MyModbus::gripperReleaseClicked()
{
    gripper_set[0] = 1;//ʹ��
    gripper_set[1] = 1;//��
}

void MyModbus::gripperClosePressed()
{
    gripper_set[0] = 1;//ʹ��
    gripper_set[1] = 0;//�պ�
}

void MyModbus::gripperCloseReleased()
{
    gripper_set[0] = 0;//ʹ��
}

void MyModbus::gripperOpenPressed()
{
    gripper_set[0] = 1;//ʹ��
    gripper_set[1] = 1;//��
}

void MyModbus::gripperOpenReleased()
{
    gripper_set[0] = 0;//ʹ��
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
