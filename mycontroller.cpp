#include "mycontroller.h"
#include "jointtheta.h"

MyController::MyController(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<JointTheta>("JointTheta");
}
