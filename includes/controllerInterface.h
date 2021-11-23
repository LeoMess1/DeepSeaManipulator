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
	//机械臂和传感器状态更新接口
public:
	virtual int updateRobotJointTheta(bool ServoStatus, JointTheta joint_theta) = 0; //机械臂关节角度反馈
	virtual int updateForceSensorData(double* force_data) = 0;  //力传感器数据反馈
	virtual int updateRobotState(double* robot_state) = 0;      //机械臂状态反馈

	//界面参数设置接口
public:
	virtual void OnClickedMoveRobotJoints(JointTheta jointTheta, double speed) = 0; //移动机械臂关节位置

	virtual void OnBnClickedVisualAproachPosButton(Frame pos, double speed)=0; //逼近目标点 posotion_,eu_
	virtual void OnBnClickedVisualAdjustRobotButton(Frame pos, double speed) = 0; //调整机械臂姿态posotion_,eu_
	virtual void OnBnClickedVisualAdjustToolButton(Frame pos, double speed) = 0; //调整抓手转角posotion_,eu_
	virtual void OnBnClickedVisualMoveToPosButton(Frame pos, double speed) = 0;  //运动到目标点posotion_,eu_
	virtual void OnBnClickedVisualCalibrationPosButton(double *visualPos) = 0;    //获取视觉标定板位置
	virtual void OnBnClickedVisualCalibrationButton()=0;       //标定

	virtual void OnBnClickedAtiBtnStart()=0; //记录负载
	virtual void OnBnClickedAtiBtnBias()=0;	 //六维力数据清零


	virtual Icommander_list * get_commander_list() = 0;
};

Controller_Interface * make_controller_interface();
#endif