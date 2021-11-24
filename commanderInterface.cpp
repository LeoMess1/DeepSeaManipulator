#include "commanderInterface.h"
#include <stdio.h>
Icommander_list* commanderInterface::instance_ = NULL;
commanderInterface* commanderInterface::commandInstance_= NULL;

Icommander_list* commanderInterface::Instance()
{
    if (NULL == instance_)
    {
        instance_ = (Icommander_list*) commandInstance_;
    }
    return instance_;
}

commanderInterface* commanderInterface::Instance_()
{
    if (NULL == commandInstance_)
    {
        commandInstance_ = new commanderInterface();
    }
    return commandInstance_;
}

commanderInterface::commanderInterface()
{
    myModbus_ =NULL;
}

void commanderInterface::init(MyModbus * myModbus)
{
    myModbus_ = myModbus;
}

int commanderInterface::displayForceSensorResult(int state)
{
    return 0;
}

int commanderInterface::displayCalibrationPosIndex(int num_Index)
{
    return 0;
}

int commanderInterface::reportError(int error, std::string error_log)
{
    return 0;
}

FILE *fp = fopen("E://points.txt","a+");
int commanderInterface::sendPosCommander(double* pos_commander)
{
//    QByteArray pos_commander_pdu =  myModbus_->generatePdu(pos_commander,myModbus_->gripper_set);
//    myModbus_->moveToPoint(pos_commander_pdu);
    memcpy(myModbus_->trajectory_planed,pos_commander,sizeof(double)*6);
    emit myModbus_->saveWriteDataSignal(pos_commander);
    fprintf(fp,"%f,%f,%f,%f,%f,%f\n",pos_commander[0],pos_commander[1],pos_commander[2],pos_commander[3],pos_commander[4],pos_commander[5]);
    return 0;
}

int commanderInterface::stopDebug_resetAlgorithm()//����������淢��ֹͣ�������㷨ָ��
{
    return 0;
}

int commanderInterface::sendPosOffset(double *offset)//����λ�ÿ������
{
    return 0;
}

int commanderInterface::sendPosOffset_error(int Joint_Index)//����λ�ÿ������������
{
    return 0;
}

int commanderInterface::isAnyErrorExist()//�����������ѯ��ϵͳ�Ƿ�����������󣬷���Ϊ0����ʾ����
{
    return 0;
}

Icommander_list * make_icommander_list()
{
    return COMMANDERINTERFACE;
}
