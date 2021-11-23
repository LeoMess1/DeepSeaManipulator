#ifndef ROBOTINTERFACE_H_H
#define ROBOTINTERFACE_H_H
#define ROBOTINTERFACE RobotInterface::Instance()
#define COMMANDER_LIST  ROBOTINTERFACE->get_commander_list()
#include "commander_list_interface.h"
#include "controllerInterface.h"

class JointTheta;
class Frame;
class ToolObject;
class ForceData;
class RobotState;
class SpaceCalculator;        //机器人空间结算方法
class SplineCalculator;       //样条曲线计算方法
class RobotAlgorithm;
class WorkSetting;
class VisualDlg;
class MotorGroupInterface;

class RobotInterface :public Controller_Interface
{
public:
	static Controller_Interface* Instance();

public:
	virtual void init(Icommander_list *list);

	//机械臂和传感器状态更新接口
public:
	virtual int updateRobotJointTheta(bool ServoStatus, JointTheta joint_theta);
	virtual int updateForceSensorData(double* force_data );
	virtual int updateRobotState(double* robot_state );

	//界面参数设置接口
public:
	virtual void OnClickedMoveRobotJoints(JointTheta jointTheta, double speed);

	virtual void OnBnClickedVisualAproachPosButton(Frame pos, double speed);
	virtual void OnBnClickedVisualAdjustRobotButton(Frame pos, double speed);
	virtual void OnBnClickedVisualAdjustToolButton(Frame pos, double speed);
	virtual void OnBnClickedVisualMoveToPosButton(Frame pos, double speed);
	virtual void OnBnClickedVisualCalibrationPosButton(double *visualPos);
	virtual void OnBnClickedVisualCalibrationButton();

	virtual void OnBnClickedAtiBtnStart();//记录负载
	virtual void OnBnClickedAtiBtnBias();


	 Icommander_list *get_commander_list();
private:
	RobotInterface();

private:
	static Controller_Interface* instance_;
	MotorGroupInterface *motorGroup_;
	SpaceCalculator  *spaceCalcualtor_;        //机器人空间结算方法
	SplineCalculator *splineCalculator_;       //样条曲线计算方法
	RobotAlgorithm* robotAlgorithm_;
	WorkSetting *workSetting_;
	VisualDlg *visual_;

	Icommander_list *list_;
};
#endif