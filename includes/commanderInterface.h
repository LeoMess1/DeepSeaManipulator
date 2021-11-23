#ifndef COMMANDERINTERFACE_H_H
#define COMMANDERINTERFACE_H_H
#define COMMANDERINTERFACE commanderInterface::Instance()
#include "commander_list_interface.h"

class commanderInterface:public Icommander_list
{

public:
	static Icommander_list* Instance();
	//��ʾ��Ϣ�ӿ�
public:
	virtual int displayForceSensorResult(int state); //����������淢����ά�����ش�С���Լ���ײ�����
	virtual int displayCalibrationPosIndex(int num_Index); //����������淢�����۱궨λ�õ����

	virtual int reportError(int error, std::string error_log); //����������汨�������Ϣ

	virtual int sendPosCommander(double* pos_commander) ;//���������е�۷��͹ؽ�λ��ָ��
	virtual int stopDebug_resetAlgorithm() ; //����������淢��ֹͣ�������㷨ָ��
	virtual int sendPosOffset(double *offset) ;//����λ�ÿ������
	virtual int sendPosOffset_error(int Joint_Index) ;//����λ�ÿ������������
	virtual int isAnyErrorExist();//�����������ѯ��ϵͳ�Ƿ�����������󣬷���Ϊ0����ʾ����

private:
	commanderInterface();

private:
	static Icommander_list* instance_;
};

#endif