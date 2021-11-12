#ifndef MYCONTROLLER_H
#define MYCONTROLLER_H

#include <QObject>

#include "controllerInterface.h"
#include "jointtheta.h"

class MyController : public QObject
{
    Q_OBJECT
public:
    explicit MyController(QObject *parent = nullptr);

    void sendJoints();

signals:

};

#endif // MYCONTROLLER_H
