#ifndef _CONTROLLER_INTERFACE_H
#define _CONTROLLER_INTERFACE_H
#define CONTROLLER_INTERFACE make_controller_interface()
#include "commander_list_interface.h"
class JointTheta;
class Frame;

class Controller_Interface
{
public:
	virtual void init(Icommander_list *list) = 0;
	//��е�ۺʹ�����״̬���½ӿ�
public:
	virtual int updateRobotJointTheta(bool ServoStatus, JointTheta joint_theta) = 0; //��е�۹ؽڽǶȷ���
	virtual int updateForceSensorData(double* force_data) = 0;  //�����������ݷ���
	virtual int updateRobotState(double* robot_state) = 0;      //��е��״̬����

	//����������ýӿ�
public:

	virtual void resetAlgrithm() = 0;
	virtual void OnClickedMoveRobotJoints(JointTheta jointTheta, double speed) = 0; //�ƶ���е�۹ؽ�λ��

	virtual void OnBnClickedVisualAproachPosButton(Frame pos, double speed)=0; //�ƽ�Ŀ��� posotion_,eu_
	virtual void OnBnClickedVisualAdjustRobotButton(Frame pos, double speed) = 0; //������е����̬posotion_,eu_
	virtual void OnBnClickedVisualAdjustToolButton(Frame pos, double speed) = 0; //����ץ��ת��posotion_,eu_
	virtual void OnBnClickedVisualMoveToPosButton(Frame pos, double speed) = 0;  //�˶���Ŀ���posotion_,eu_
	virtual void OnBnClickedVisualCalibrationPosButton(double *visualPos) = 0;    //��ȡ�Ӿ��궨��λ��
	virtual void OnBnClickedVisualCalibrationButton()=0;       //�궨

	virtual void OnBnClickedAtiBtnStart()=0; //��¼����
	virtual void OnBnClickedAtiBtnBias()=0;	 //��ά����������


	virtual Icommander_list * get_commander_list() = 0;
};

Controller_Interface * make_controller_interface();
#endif