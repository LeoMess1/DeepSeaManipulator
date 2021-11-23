#include "commanderInterface.h"
Icommander_list* commanderInterface::instance_ = NULL;

Icommander_list* commanderInterface::Instance()
{
	if (NULL == instance_)
	{
		instance_ = new commanderInterface();
	}
	return instance_;
}

commanderInterface::commanderInterface()
{

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

int commanderInterface::sendPosCommander(double* pos_commander)
{
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