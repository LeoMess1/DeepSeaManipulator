#include "mycontroller.h"
#include "jointtheta.h"

MyController::MyController(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<JointTheta>("JointTheta");
}

void MyController::start_trajectory_plan(JointTheta jointTheta, double jointSpeed)
{
    CONTROLLER_INTERFACE->OnClickedMoveRobotJoints(jointTheta,jointSpeed);

}
