#ifndef _TOOLOBJECT_ROBOT_H_
#define _TOOLOBJECT_ROBOT_H_
#include <string>
using namespace std;

class ToolObject
{
public:
	ToolObject(string toolObjectName);
	ToolObject& operator=(const ToolObject &src);

	int getRobotSixEndPos(Frame robotTcpPos, Frame& robotSixEndPos);
	int getRobotTcpPos(Frame robotSixEndPos, Frame& robotTcpPos);
	int getOffsetTcp(Frame offsetTcp, Frame& robotTcpPos);
	int calculateToolObject();
	int saveToolObject();
	int setToolObjectParam(Vector3 toolPos, EulerAngle toolRotation);

public:
	string toolObjectName_;//工具坐标系名称

	EulerAngle toolRotation_;//工具坐标系相对第6轴坐标系的旋转欧拉角
	bool enableFlag_;//标志位，表示该坐标系是否启用,初始化为false
	Frame toolPos_;  //工具在第6轴坐标系下的位置

	int teachPosNum_;//示教工具坐标系需要的位置点的个数
	std::vector<Frame> teachPos_; //示教点位置
	std::vector<Vector3> calculatePos_;//计算得到的工具坐标系位置

	double meanError_;//计算得到的工具坐标系的平均误差
	double maxError_;//计算得到的工具坐标系的最大误差
};

#endif;