#ifndef _COMMANDER_LIST_INTERFACE_H //
#define _COMMANDER_LIST_INTERFACE_H
#define ICOMMANDER_INTERFACE make_icommander_list()
#include <string>
class JointTheta;

class Icommander_list
{

   // virtual void init(MyModbus * myModbus) = 0;
	//显示消息接口
public:
	virtual int displayForceSensorResult(int state) = 0; //控制器向界面发送六维力负载大小，以及碰撞检测结果
	virtual int displayCalibrationPosIndex(int num_Index) = 0; //控制器向界面发送手眼标定位置点个数

	virtual int reportError(int error, std::string error_log)=0; //控制器向界面报告错误信息

	virtual int sendPosCommander(bool servo,JointTheta pos_commander) = 0;//控制器向机械臂发送关节位置指令
	virtual int stopDebug_resetAlgorithm() = 0; //控制器向界面发送停止控制器算法指令
	virtual int sendPosOffset(double *offset) = 0;//发送位置控制误差
	virtual int sendPosOffset_error(int Joint_Index) = 0;//发送位置控制误差过大错误
	virtual int isAnyErrorExist() = 0;//控制器向界面询问系统是否存在致命错误，返回为0，表示无误

	
};

Icommander_list * make_icommander_list();
#endif //_COMMANDER_LIST_INTERFACE_H


