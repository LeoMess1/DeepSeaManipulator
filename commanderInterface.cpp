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

int commanderInterface::stopDebug_resetAlgorithm()//控制器向界面发送停止控制器算法指令
{
    return 0;
}

int commanderInterface::sendPosOffset(double *offset)//发送位置控制误差
{
    return 0;
}

int commanderInterface::sendPosOffset_error(int Joint_Index)//发送位置控制误差过大错误
{
    return 0;
}

int commanderInterface::isAnyErrorExist()//控制器向界面询问系统是否存在致命错误，返回为0，表示无误
{
    return 0;
}

Icommander_list * make_icommander_list()
{
    return COMMANDERINTERFACE;
}
