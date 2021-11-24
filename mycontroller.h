#ifndef MYCONTROLLER_H
#define MYCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QMetaType>
#include <QThread>

#include "controllerInterface.h"
#include "jointtheta.h"

class MyController : public QObject
{
    Q_OBJECT
    void startTrajectoryPlan();

public:
    explicit MyController(QObject *parent = nullptr);

    void startTrajectoryPlan(JointTheta theta, double speed);

signals:

};

#endif // MYCONTROLLER_H
