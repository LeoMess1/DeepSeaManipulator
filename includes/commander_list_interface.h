#ifndef _COMMANDER_LIST_INTERFACE_H //
#define _COMMANDER_LIST_INTERFACE_H
#define ICOMMANDER_INTERFACE make_icommander_list()
#include <string>
class JointTheta;

class Icommander_list
{

   // virtual void init(MyModbus * myModbus) = 0;
	//��ʾ��Ϣ�ӿ�
public:
	virtual int displayForceSensorResult(int state) = 0; //����������淢����ά�����ش�С���Լ���ײ�����
	virtual int displayCalibrationPosIndex(int num_Index) = 0; //����������淢�����۱궨λ�õ����

	virtual int reportError(int error, std::string error_log)=0; //����������汨�������Ϣ

	virtual int sendPosCommander(bool servo,JointTheta pos_commander) = 0;//���������е�۷��͹ؽ�λ��ָ��
	virtual int stopDebug_resetAlgorithm() = 0; //����������淢��ֹͣ�������㷨ָ��
	virtual int sendPosOffset(double *offset) = 0;//����λ�ÿ������
	virtual int sendPosOffset_error(int Joint_Index) = 0;//����λ�ÿ������������
	virtual int isAnyErrorExist() = 0;//�����������ѯ��ϵͳ�Ƿ�����������󣬷���Ϊ0����ʾ����

	
};

Icommander_list * make_icommander_list();
#endif //_COMMANDER_LIST_INTERFACE_H


