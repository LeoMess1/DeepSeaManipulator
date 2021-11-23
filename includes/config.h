#ifndef _CONFIG_ROBOT_H_
#define _CONFIG_ROBOT_H_
#include "typedef.h"
#include <string.h>
#define CONFIG Config::Instance()
#define ROBOTMOTOR_NUMBER  6
class Config
{
public:
    static Config* Instance();
    ~Config();

public:
    int load();
    int save();
	int get_robot_type(){ return robot_type_; };
	int get_motor_type(){ return motor_type_; };
	bool is_CE_mode(){ return is_CE_mode_; };

public:
    // 电机归零点的零位
    struct Deviation
    {
        Deviation()
        {
            memset(motors_, 0x00, sizeof(double)*ROBOT_JOINT_NUMBER);
			memset(motorreductionRatio_,0x00,sizeof(double)*ROBOTMOTOR_NUMBER);
        }
        double motors_[ROBOT_JOINT_NUMBER];
		double motorreductionRatio_[ROBOTMOTOR_NUMBER];
    };

public:
    // IO板的个数
    int ioBoardNumber_;
    Deviation deviation_;
	int robot_type_;
	int motor_type_;
	bool is_CE_mode_;
private:
    explicit Config();
    static Config* instance_;
};

#endif
