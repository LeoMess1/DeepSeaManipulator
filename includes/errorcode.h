#ifndef ERRORCODE_H_H
#define ERRORCODE_H_H

#include "logging.h"
#include <stdio.h>
#include <map>
#include  <string>

#if _MSC_VER >= 1600  
#pragma execution_character_set("utf-8") 
#endif

#define RETURN_LOWER_LEVEL_VALUE(value, functionName) \
{ \
if (ERROR_CODE::isFatal(value)) \
	{ \
	LOG(LOG_FATAL, functionName) << "error number is " << value << std::endl; \
	LOG_FLUSH; \
	PRINT(LOG_FATAL, functionName) << "error number is " << value << std::endl; \
	} \
if (ERROR_CODE::isError(value)) \
	{ \
	LOG(LOG_ERROR, functionName) << "error number is " << value << std::endl; \
	LOG_FLUSH; \
	PRINT(LOG_ERROR, functionName) << "error number is " << value << std::endl; \
	} \
if (ERROR_CODE::isWarning(value)) \
	{ \
	LOG(LOG_WARNING, functionName) << "error number is " << value << std::endl; \
	PRINT(LOG_WARNING, functionName) << "error number is " << value << std::endl; \
	} \
if (ERROR_CODE::isInfo(value)) \
	{ \
	LOG(LOG_INFO, functionName) << "error number is " << value << std::endl; \
	} \
	return value; \
}



// º¯Êý·µ»ØÕýÈ· 0
// LOG_FATAL ÕŒÓÃŽíÎó±àºÅ1-5000
// LOG_ERROR ÕŒÓÃŽíÎó±àºÅ5001-10000
// LOG_WARNING ÕŒÓÃŽíÎó±àºÅ10001-15000
// LOG_INFO ÕŒÓÃŽíÎó±àºÅ15001-20000
struct ERROR_CODE
{
	static int isFatal(int errorCode)
	{
		if ((errorCode >= 1) && (errorCode <= 5000))
		{
			return ROBOT_TRUE;
		}
		else
		{
			return ROBOT_FALSE;
		}
	}

	static int isError(int errorCode)
	{
		if ((errorCode >= 5001) && (errorCode <= 10000))
		{
			return ROBOT_TRUE;
		}
		else
		{
			return ROBOT_FALSE;
		}
	}

	static int isWarning(int errorCode)
	{
		if ((errorCode >= 10001) && (errorCode <= 15000))
		{
			return ROBOT_TRUE;
		}
		else
		{
			return ROBOT_FALSE;
		}
	}

	static int isInfo(int errorCode)
	{
		if ((errorCode >= 15001) && (errorCode <= 20000))
		{
			return ROBOT_TRUE;
		}
		else
		{
			return ROBOT_FALSE;
		}
	}

	static int lowerLevelReturnValue(int value, char *functionName)
	{
		if (isFatal(value))
		{
			LOG(LOG_FATAL, functionName) << "error number is " << value << std::endl;
			LOG_FLUSH;
			PRINT(LOG_FATAL, functionName) << "error number is " << value << std::endl;
		}
		if (isError(value))
		{
			LOG(LOG_ERROR, functionName) << "error number is " << value << std::endl;
			LOG_FLUSH;
			PRINT(LOG_ERROR, functionName) << "error number is " << value << std::endl;
		}
		if (isWarning(value))
		{
			LOG(LOG_WARNING, functionName) << "error number is " << value << std::endl;
			PRINT(LOG_WARNING, functionName) << "error number is " << value << std::endl;
		}
		if (isInfo(value))
		{
			LOG(LOG_INFO, functionName) << "error number is " << value << std::endl;
			PRINT(LOG_INFO, functionName) << "error number is " << value << std::endl;
		}
		return value;
	}


	/************************************************ SUCCESSED ************************************************/
	static const int SUCCEEDED = 0;
	//
	static const int  ERROR_CODE_INVERSEKINEMATIC_ONE_NO_SOLUTION = 594;// 逆运动学无解
	static const int  ERROR_CODE_INVERSEKINEMATIC_TWO_NO_SOLUTION = 595;// 逆运动学无解
	static const int  ERROR_CODE_INVERSEKINEMATIC_THREE_NO_SOLUTION = 596;// 逆运动学无解
	static const int  ERROR_CODE_INVERSEKINEMATIC_FOUR_NO_SOLUTION = 597;// 逆运动学无解
	static const int  ERROR_CODE_INVERSEKINEMATIC_FIVE_NO_SOLUTION = 598;// 逆运动学无解
	static const int  ERROR_CODE_INVERSEKINEMATIC_SIX_NO_SOLUTION = 599;// 逆运动学无解

	static const int  ERROR_CODE_INVERSEKINEMATIC_NO_SOLUTION = 600;// 逆运动学无解
	static const int  ERROR_CODE_FIRST_AXIS_ECEEDING_RANGE = 601;// 机器人一轴越界#
	static const int  ERROR_CODE_SECOND_AXIS_ECEEDING_RANGE = 602;// 机器人二轴越界#
	static const int  ERROR_CODE_THIRD_AXIS_ECEEDING_RANGE = 603;// 机器人三轴越界#
	static const int  ERROR_CODE_FOURTH_AXIS_ECEEDING_RANGE = 604;// 机器人四轴越界#
	static const int  ERROR_CODE_FIFTH_AXIS_ECEEDING_RANGE = 605;// 机器人五轴越界#
	static const int  ERROR_CODE_SIXTH_AXIS_ECEEDING_RANGE = 606;// 机器人六轴越界#
	static const int  ERROR_CODE_MATRIX_TRANSFORM_ERROR = 607;// 齐次变换矩阵计算出错#
	static const int  ERROR_HOMOGENOUS_MATRIX_TRACE_LESS_THEN_ZERO = 608;// 齐次变换矩阵的迹小于零#
	static const int  ERROR_CODE_MATRIX_DETERMINANT_IS_ZERO = 609;// 矩阵行列式值为零#
	static const int  ERROR_CODE_INTERPOLATE_MATRIX_ERROR = 610;//插值矩阵计算错误#
	static const int  ERROR_CODE_SPLINE_FIRST_PART_CALCULATED_ERROR = 612; // 样条曲线第一段计算出错#
	static const int  ERROR_CODE_SPLINE_CALCULATE_ERROR = 613;//样条曲线计算错误# 
	static const int ERROR_CODE_WORKOBJECT_POS_WARNING = 614; // 工件坐标系点未示教或者原点和X点重合#  
	static const int ERROR_CODE_WORKOBJECT_NOT_EXIT = 615; // 工件坐标系不存在# 
	static const int ERROR_CODE_WORKOBJECT_YDERECTION_WARNING = 616; // 工件坐标系Y轴方向未设定#
	static const int  ERROR_CODE_DISTANCE_VERY_BIG = 617;// 理论位置和实际位置差距太大#
	static const int  ERROR_CODE_WORK_PATTERN_NOT_EXIT = 618;// 工作模式不存在#
	static const int  ERROR_CODE_WAITING_WHILE_ROBOT_RUNNING = 619;// 机器人正处在运行当中，而此时却有等待信号#
	static const int  ERROR_CODE_QUATERNION_ABNORMAL = 620;//四元数模为零，在求逆过程中出错#
	static const int  ERROR_CODE_STOP_DEBUGGER_WHILE_RUNNING = 621;// 机器人正常运行时突然停止debug#
	static const int  ERROR_CODE_EXECUTE_STEPPING = 622;          // 机器人正常运行时突然按单步         
	static const int  ERROR_CODE_ROBOT_SINGULAR_POINT = 624;         //机器人奇异点
	static const int  ERROR_CODE_INTERPOLATION_BREAK = 625;         //插值突变
        static const int  ERROR_CODE_MOVING_GRAP_TIME_NOT_ENOUGH = 626; //机器人运动抓取时间不足
		static const int  ERROR_CODE_MATRIX_IS_NOT_SQUARE = 627;    //矩阵不为方阵
		static const int  ERROR_CODE_MATRIX_POINT_MULTIPLY_DIMENSION_NOT_MATCH = 628;    //矩阵点乘维度不相等
		static const int  ERROR_CODE_TEST_WOBJ_WHILE_RUNNING = 629;   //程序运行过程中测试工件坐标系
                static const int  ERROR_CODE_CALCULATE_QUANION_DIRECTION_FAILED = 630;    //四元数计算方向失败
                static const int  ERROR_CODE_TOOLOBJECT_NOT_EXIT = 631;

		static const int  ERROR_CODE_485_OPEN_FAIL = 900;               //#485打开失败#
		static const int  ERROR_CODE_485_SETATTRIBUTE_FAILT = 901;               //#485设置属性失败#
		static const int  ERROR_CODE_485_SEND_FAIL_WARNING = 902;               //#485发送失败#
		static const int  ERROR_CODE_485_READ_FIAL_WARNING = 903;          //#485接收失败#
		static const int  ERROR_CODE_485_CRC_TEST_WARNING = 904;          //#485CRC校验失败#
		static const int  ERROR_CODE_IO_BOARD_COMMUNICATION_WARNING = 905;                //#IO板通信中断#
		static const int  ERROR_CODE_PULSE_BOARD_COMMUNICATION_ERROR = 906;                //#脉冲板通信中断#
		static const int  ERROR_CODE_FLEXPENDANT_COMMUNICATION_ERROR = 907;           //#示教器通信中断#
		static const int  ERROR_CODE_SAFETY_SIGNSL_BE_TRIGGERED_INFO = 908;           //#安全信号被触发！#
		static const int  ERROR_CODE_IO_NUM_ZERO = 909;                      //IO板个数为0
		static const int  ERROR_CODE_CAMERA_COMMUNICATION_ERROR = 910;                      //#camera通信中断#
		static const int  ERROR_CODE_ORIENT_DIFFER_WITH_CAMERA_TEACH_POS = 911;                      //ORIENT_DIFFER_WITH_CAMERA_TEACH_POS
		static const int  ERROR_CODE_CAMERA_TEACH_POS_NOT_DEFINE = 912;                      //CAMERA_TEACH_POS_NOT_DEFINE

		static const int  ERROR_CODE_SOCKET_WSAStartup_CREATE_FAIL = 1200;          //WSAStartup 创建出错#
		static const int  ERROR_CODE_SOCKET_CREATE_FAIL = 1201;          //socket创建出错#
		static const int  ERROR_CODE_SOCKET_BIND_FAIL = 1202;          //socket绑定出错#
		static const int  ERROR_CODE_SOCKET_SET_BLOCK_FAIL = 1203;          //socket设置阻塞出错#
		static const int  ERROR_CODE_EVENT_CREATE_FAIL = 1204;          //创建事件出错#
		static const int  ERROR_CODE_THREAD_CREATE_FAIL = 1205;          //线程创建失败#
		static const int  ERROR_CODE_THREAD_RESUME_FAIL = 1206;          //线程重启出错#
		static const int  ERROR_CODE_THREAD_SUSPEND_FAIL = 1207;          //线程挂起失败#
		static const int  ERROR_CODE_THREAD_TERMINATE_FAIL = 1208;          //杀死线程失败#
		static const int  ERROR_CODE_FILE_OPEN_FAIL = 1209;          //文件打开失败#
		static const int  ERROR_CODE_FILE_CLOSE_FAIL = 1210;          //文件关闭失败#
		static const int  ERROR_CODE_MUTEX_CREATE_FAIL = 1211;          // mutex 创建失败#
		static const int  ERROR_CODE_SET_SCHEDULER_FAIL = 1212;          // 设置scheduler失败#
		static const int  ERROR_CODE_SET_MEM_LOCK_FAIL = 1213;          //设置锁内存失败#
		static const int  ERROR_FUNCTION_PARAMETERS_ERROR = 1214;          // 函数参数出错#
		static const int  ERROR_CODE_PROJECT_NOT_EXIST = 1215;          // 文件不存在#
	//     /************************************************ LOG_FATAL ************************************************/
	//     static const int ERROR_CODE_SOCKET_WSAStartup_CREATE_FAIL        = 1;       // WSAStartup ŽŽœš³öŽí
	//     static const int ERROR_CODE_SOCKET_CREATE_FAIL                   = 2;       // socket ŽŽœš³öŽí
	//     static const int ERROR_CODE_SOCKET_BIND_FAIL                     = 3;       // socket °ó¶š³öŽí
	//     static const int ERROR_CODE_SOCKET_SET_BLOCK_FAIL                = 4;       // socket ÉèÖÃ×èÈû³öŽí
	//     static const int ERROR_CODE_EVENT_CREATE_FAIL                    = 5;       // ŽŽœšÊÂŒþ³öŽí
	//     static const int ERROR_CODE_THREAD_CREATE_FAIL                   = 6;       // Ïß³ÌŽŽœšÊ§°Ü
	//     static const int ERROR_CODE_THREAD_RESUME_FAIL                   = 7;       // Ïß³ÌÖØÆô³öŽí
	//     static const int ERROR_CODE_THREAD_SUSPEND_FAIL                  = 8;       // Ïß³Ì¹ÒÆðÊ§°Ü
	//     static const int ERROR_CODE_FILE_OPEN_FAIL                       = 9;       // ÎÄŒþŽò¿ªÊ§°Ü
	//     static const int ERROR_CODE_FILE_CLOSE_FAIL                      = 10;      // ÎÄŒþ¹Ø±ÕÊ§°Ü
	//     static const int ERROR_CODE_485_OPEN_FAIL                        = 11;      // 485Žò¿ªÊ§°Ü
	//     static const int ERROR_CODE_485_SETATTRIBUTE_FAILT               = 12;      // 485ÉèÖÃÊôÐÔÊ§°Ü
	//     static const int ERROR_CODE_MUTEX_CREATE_FAIL                    = 13;      // mutex ŽŽœšÊ§°Ü
	//     static const int ERROR_CODE_SET_SCHEDULER_FAIL                   = 14;      // ÉèÖÃschedulerÊ§°Ü
	//     static const int ERROR_CODE_SET_MEM_LOCK_FAIL                    = 15;      // ÉèÖÃËøÄÚŽæÊ§°Ü
	//     static const int ERROR_CODE_MATRIX_DETERMINANT_IS_ZERO           = 16;      // ŸØÕóÐÐÁÐÊœÖµÎªÁã
	//     static const int ERROR_CODE_MATRIX_TRANSFORM_ERROR               = 17;      // ÆëŽÎ±ä»»ŸØÕóŒÆËã³öŽí
	//     static const int ERROR_CODE_INTERPOLATE_MATRIX_ERROR             = 18;      // ²åÖµŸØÕóŒÆËãŽíÎó
	//     static const int ERROR_CODE_SPLINE_SEGMENT_CALCULATE_ERROR       = 19;      // ÑùÌõÇúÏß×î¶àÈý¶Î
	//     static const int ERROR_CODE_SPLINE_FIRST_PART_CALCULATED_ERROR   = 20;      // ÑùÌõÇúÏßµÚÒ»¶ÎŒÆËã³öŽí
	//     static const int ERROR_HOMOGENOUS_MATRIX_TRACE_LESS_THEN_ZERO    = 21;      // ÆëŽÎ±ä»»ŸØÕóµÄŒ£Ð¡ÓÚÁã
	//     static const int ERROR_FUNCTION_PARAMETERS_ERROR                 = 22;      // º¯Êý²ÎÊý³öŽí
	//     static const int ERROR_CODE_PULSE_BOARD_COMMUNICATION_ERROR      = 23;      // Âö³å°åÍšÐÅÖÐ¶Ï
	//     static const int ERROR_CODE_FLEXPENDANT_COMMUNICATION_ERROR      = 24;      // ÊŸœÌÆ÷ÍšÐÅÖÐ¶Ï
	//     static const int ERROR_CODE_THREAD_TERMINATE_FAIL                = 25;      // É±ËÀÏß³ÌÊ§°Ü
	//     /************************************************ LOG_ERROR ************************************************/
	// 
	//          
	//     /*********************************************** LOG_WARNING ***********************************************/
	//     static const int ERROR_CODE_INVERSEKINEMATIC_NO_SOLUTION         = 10001;      // ÄæÔË¶¯Ñ§ÎÞœâ
	//     static const int ERROR_CODE_FIRST_AXIS_ECEEDING_RANGE            = 10002;      // »úÆ÷ÈËÒ»ÖáÔœœç
	//     static const int ERROR_CODE_SECOND_AXIS_ECEEDING_RANGE           = 10003;      // »úÆ÷ÈË¶þÖáÔœœç
	//     static const int ERROR_CODE_THIRD_AXIS_ECEEDING_RANGE            = 10004;      // »úÆ÷ÈËÈýÖáÔœœç
	//     static const int ERROR_CODE_FOURTH_AXIS_ECEEDING_RANGE           = 10005;      // »úÆ÷ÈËËÄÖáÔœœç
	//     static const int ERROR_CODE_FIFTH_AXIS_ECEEDING_RANGE            = 10006;      // »úÆ÷ÈËÎåÖáÔœœç
	//     static const int ERROR_CODE_SIXTH_AXIS_ECEEDING_RANGE            = 10007;      // »úÆ÷ÈËÁùÖáÔœœç
	//     static const int ERROR_CODE_STOP_DEBUGGER_WHILE_RUNNING          = 10008;      // »úÆ÷ÈËÕý³£ÔËÐÐÊ±Í»È»Í£Ö¹debug£¬ÊôÓÚWARNINGÐÅÏ¢
	//     static const int ERROR_CODE_WORK_PATTERN_NOT_EXIT                = 10009;      // ¹€×÷Ä£Êœ²»ŽæÔÚ
	//     static const int ERROR_CODE_QUATERNION_ABNORMAL                  = 10010;      // ËÄÔªÊýÄ£ÎªÁã£¬ÔÚÇóÄæ¹ý³ÌÖÐ³öŽí
	//     static const int ERROR_CODE_WAITING_WHILE_ROBOT_RUNNING          = 10011;      // »úÆ÷ÈËÕýŽŠÔÚÔËÐÐµ±ÖÐ£¬¶øŽËÊ±ÈŽÓÐµÈŽýÐÅºÅ
	//     static const int ERROR_CODE_SPLINE_CALCULATE_ERROR               = 10012;      // ÑùÌõÇúÏßŒÆËãŽíÎó
	//     static const int ERROR_CODE_PROJECT_NOT_EXIST                    = 10013;      // ÎÄŒþ²»ŽæÔÚ
	//     static const int ERROR_CODE_485_SEND_FAIL_WARNING                = 10014;      // 485·¢ËÍÊ§°Ü
	//     static const int ERROR_CODE_485_READ_FIAL_WARNING                = 10015;      // 485œÓÊÕÊ§°Ü
	//     static const int ERROR_CODE_485_CRC_TEST_WARNING                 = 10016;      // 485CRCÐ£ÑéÊ§°Ü
	//     static const int ERROR_CODE_IO_BOARD_COMMUNICATION_WARNING       = 10017;      // IO°åÍšÐÅÖÐ¶Ï
	// 	static const int ERROR_CODE_WORKOBJECT_TRANSLATE_WARNING         = 10018;      // ¹€Œþ×ø±êÏµ±ä»»ŽíÎó
	// 	static const int ERROR_CODE_DISTANCE_VERY_BIG                    = 10019;      // ÀíÂÛÎ»ÖÃºÍÊµŒÊÎ»ÖÃ²îŸàÌ«Žó
	// 	/************************************************* LOG_INFO ************************************************/
	//     static const int ERROR_CODE_EXECUTE_STEPPING                     = 15001;      // È«ËÙÔËÐÐ¹ý³ÌÖÐµã»÷stepping
};
//std::map<int, std::string>  robot_report_info;
// 
// struct   message_report
// {
// 	int                         message_source;    //±íÊŸÊÇÄÄÀïµÄŽíÎó
// 	int                         message_level;     //±íÊŸÏûÏ¢µÄµÈŒ¶
// 	int                         message_ack;       //ÊÇ·ñÐèÒª»ØÓŠ
// 	Robot_report_code           message_code;      //ŽíÎóÂë
// 	std::string                 message_buf;  //ŽíÎóÐÅÏ¢ÄÚÈÝ
// ;
// };
enum  Info_level
{
	ROBOT_FAULT = 1,//机器人故障
	ROBOT_ERROR = 2,//机器人可恢复性错误
	ROBOT_INFO = 4    //机器人一般性信息
};
enum Info_source
{
	ROBOT_SYSTEM = 0,  //来自系统
	ROBOT_PARSE,       //解析出错
	ROBOT_RUNTIME,     //运行出错
	ROBOT_ALGORITHM,   //算法出错
	ROBOT_IO,          //IO板出错
	ROBOT_PULSE        //脉冲板出错
};
struct robot_report_info
{
	Info_source   message_source;
	Info_level    message_level;
	int           message_code;
	std::string   message_buf;
	int   message_buf_length;
};

class robot_info
{
public:
	robot_info();
	~robot_info();
	static robot_info* instance();
	static std::string getInfoBuf(int infoCode);


private:
	static std::map<int, std::string> infoMap;
	static robot_info*    instance_;
};

#endif
