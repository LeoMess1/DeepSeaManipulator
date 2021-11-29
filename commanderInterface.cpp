#include "commanderInterface.h"
#include <stdio.h>
#include"controllerInterface.h"

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
    CONTROLLER_INTERFACE->resetAlgrithm();
    return 0;
}

FILE *fp = fopen("E://points.txt","a+");
int commanderInterface::sendPosCommander(bool servo,JointTheta pos_commander)
{
//    QByteArray pos_commander_pdu =  myModbus_->generatePdu(pos_commander,myModbus_->gripper_set);
//    myModbus_->moveToPoint(pos_commander_pdu);
//    myModbus_->joints_set = myModbus_->JOINTS_SET_ENABLE_CLOSED_LOOP;
    myModbus_->system_server = servo;
    double pos[6]={0};
    pos[0] =  pos_commander.theta1_;
    pos[1] =  pos_commander.theta2_;
    pos[2] =  pos_commander.theta3_;
    pos[3] =  pos_commander.theta4_;
    pos[4] =  pos_commander.theta5_;
    pos[5] =  pos_commander.theta6_;
    memcpy(myModbus_->trajectory_planed,pos,sizeof(double)*6);
    myModbus_->saveDataToWrite(pos);//�洢��Ҫ���͵ĵ�
    fprintf(fp,"%f,%f,%f,%f,%f,%f\n",pos[0],pos[1],pos[2],pos[3],pos[4],pos[5]);
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
    //��ѯ��բ״̬��ϵͳ״̬

    return 0;
}

Icommander_list * make_icommander_list()
{
    return COMMANDERINTERFACE;
}
