#ifndef _FRAME_ROBOT_H_
#define _FRAME_ROBOT_H_

#include "jointtheta.h"
#include "quaternion.h"
#include "transformmatrix.h"

class Frame
{
public:
    Frame();
    Frame(const RobotTMatrix &T);
    Frame(CartesianPosition pos, Quaternion quaternion);
    Frame(const Frame &frame);
    Frame& operator=(const Frame &src);
    Frame operator*(const Frame &frame);
    Frame home();
    Frame inverse() const;
    RobotMatrix getMatrix();

	Frame euToQua();
    static double linearDistance(const Frame &src, const Frame &dst);
    static double eulerDistance(const Frame &src, const Frame &dst);
    static Frame middleFrameInFrame(const Frame &src, const Frame &dst, double ratio);

public:
    static const int FRAME_ELEMENT_NUM = 7;

    CartesianPosition pos_;
    Quaternion quaternion_;
	EulerAngle eu_;
};

#endif
