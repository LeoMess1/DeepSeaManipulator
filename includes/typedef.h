#ifndef TYPEDEF_H_H
#define TYPEDEF_H_H

#pragma warning (disable:4290)

#define IN  
#define OUT  
#define INOUT  

#define Robot120                       1
#define Robot180                       0
#define MotorDechuang                  2
#define MotorTaida                     1
#define MotorSanyang                   0

#ifdef WIN32    
#define ROBOT_UINT64               unsigned __int64 
#define ROBOT_INT64                __int64 
#else
#define ROBOT_UINT64               unsigned long long
#define ROBOT_INT64                long long
#endif
#define ROBOT_BOOL                 unsigned int
#define ROBOT_TRUE                 1
#define ROBOT_FALSE                0
#define ROBOT_EPSILON              2.2204460492503131e-014
#define ROBOT_PI                   (double(3.1415926535897932384626433832795))
#define ROBOT_PI_DIVIDE_TWO         ROBOT_PI / 2
#define ROBOT_PI_DIVIDE_FOUR        ROBOT_PI / 4
#define ROBOT_TIMER_PERIOD         ((double)0.004)
#define ROBOT_MINIMUM_DISTANCE     0.001
#define ROBOT_MINIMUM_SPEED        0.001

#define ROBOT_JOINT_NUMBER         6
#define ROBOT_R_DIMENSION          3

#define ROBOT_JOINT_ONE            0
#define ROBOT_JOINT_TWO            1
#define ROBOT_JOINT_THREE          2
#define ROBOT_JOINT_FOUR           3
#define ROBOT_JOINT_FIVE           4
#define ROBOT_JOINT_SIX            5

#define ROBOT_ENUM_BEGIN          0x0000
#define ROBOT_ENUM_END            0xFFFF

#define ROBOT_ERROR_CODE_END      0xFFFF

#define ROBOT_MAX_DOUBLE32        ((double)3.402823466e+38)

#define ROBOT_JOINT_ONE_THETA_MAX        1.04//60
#define ROBOT_JOINT_ONE_THETA_MIN       -1.04 //-60
#define ROBOT_JOINT_TWO_THETA_MAX        0.523 //30
#define ROBOT_JOINT_TWO_THETA_MIN      -1.57//-90
#define ROBOT_JOINT_THREE_THETA_MAX     1.57//90
#define ROBOT_JOINT_THREE_THETA_MIN     -0.523 //-30
#define ROBOT_JOINT_FOUR_THETA_MAX    2.61 //150
#define ROBOT_JOINT_FOUR_THETA_MIN    -2.61 //-150
#define ROBOT_JOINT_FIVE_THETA_MAX     0 //0
#define ROBOT_JOINT_FIVE_THETA_MIN     -1.39//  -80
#define ROBOT_JOINT_SIX_THETA_MAX      3.14 //180
#define ROBOT_JOINT_SIX_THETA_MIN      -3.14 //-180
#define ROBOT_JOINT_DISTANCE_BETWEEN_TWO_AND_THREE_AXIS 0.50 // 25°„ ¡Ùµ„”‡¡ø
#define ROBOT_JOINT_THETA_MARGIN        0.052

#define ROBOT_MILLIMETER_RATIO          1000

#define VARIABLE_CONF_FILE_NAME                   "robot.conf"
#define PROJECTS_CONF_FILE_NAME                   "projects.conf"
#define WORKOBJECT_CONF_FILE_NAME                   "wobj.conf"
#define TOOLOBJECT_CONF_FILE_NAME                   "tobj.conf"

#ifndef WIN32
#define Sleep(num) usleep(num*1000)
#endif

#define FOURCHAR_TO_INT(buffer) (int)(((((unsigned int)(*buffer)) << 24) & 0xFF000000) \
    + (((unsigned int)(*(buffer + 1)) << 16) & 0x00FF0000) \
    + (((unsigned int)(*(buffer + 2)) << 8) & 0x0000FF00) \
    + (((unsigned int)(*(buffer + 3)) << 0) & 0x000000FF))

enum DeviceType
{
    DEVICE_TYPE_FLEXPENDANT = 0x00,
    DEVICE_TYPE_IPC         = 0x10,
    DEVICE_TYPE_PULSE_BOARD = 0x20,
    DEVICE_TYPE_IO_BOARD    = 0x30
};

#endif
