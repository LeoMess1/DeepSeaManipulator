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