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

public:
    explicit MyController(QObject *parent = nullptr);

    void start_trajectory_plan(JointTheta jointTheta, double speed);

signals:

private:


};

#endif // MYCONTROLLER_H
