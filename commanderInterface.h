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
	//显示消息接口
public:
	virtual int displayForceSensorResult(int state); //控制器向界面发送六维力负载大小，以及碰撞检测结果
	virtual int displayCalibrationPosIndex(int num_Index); //控制器向界面发送手眼标定位置点个数

	virtual int reportError(int error, std::string error_log); //控制器向界面报告错误信息

    virtual int sendPosCommander(bool servo,JointTheta pos_commander) ;//控制器向机械臂发送关节位置指令
	virtual int stopDebug_resetAlgorithm() ; //控制器向界面发送停止控制器算法指令
	virtual int sendPosOffset(double *offset) ;//发送位置控制误差
	virtual int sendPosOffset_error(int Joint_Index) ;//发送位置控制误差过大错误
	virtual int isAnyErrorExist();//控制器向界面询问系统是否存在致命错误，返回为0，表示无误

private:
	commanderInterface();
    MyModbus * myModbus_;
private:
	static Icommander_list* instance_;
    static commanderInterface* commandInstance_;
};

#endif
