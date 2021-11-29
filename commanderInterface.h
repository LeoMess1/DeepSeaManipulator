#ifndef COMMANDERINTERFACE_H_H
#define COMMANDERINTERFACE_H_H
#define COMMANDERINTERFACE commanderInterface::Instance()
#define COMMANDER_INTERFACE commanderInterface::Instance_()

#include "commander_list_interface.h"
#include "mymodbus.h"
#include "jointtheta.h"

class commanderInterface:public Icommander_list
{

public:
	static Icommander_list* Instance();
    static commanderInterface* Instance_();
    void init(MyModbus * myModbus);
	//��ʾ��Ϣ�ӿ�
public:
	virtual int displayForceSensorResult(int state); //����������淢����ά�����ش�С���Լ���ײ�����
	virtual int displayCalibrationPosIndex(int num_Index); //����������淢�����۱궨λ�õ����

	virtual int reportError(int error, std::string error_log); //����������汨�������Ϣ

    virtual int sendPosCommander(bool servo,JointTheta pos_commander) ;//���������е�۷��͹ؽ�λ��ָ��
	virtual int stopDebug_resetAlgorithm() ; //����������淢��ֹͣ�������㷨ָ��
	virtual int sendPosOffset(double *offset) ;//����λ�ÿ������
	virtual int sendPosOffset_error(int Joint_Index) ;//����λ�ÿ������������
	virtual int isAnyErrorExist();//�����������ѯ��ϵͳ�Ƿ�����������󣬷���Ϊ0����ʾ����

private:
	commanderInterface();
    MyModbus * myModbus_;
private:
	static Icommander_list* instance_;
    static commanderInterface* commandInstance_;
};

#endif
