#ifndef _JOINTTHETA_ROBOT_H_
#define _JOINTTHETA_ROBOT_H_

class DirectionCos
{
public:
    DirectionCos(double xCos = 0.0, double yCos = 0.0, double zCos = 0.0);
    DirectionCos(const DirectionCos &DCos);
    DirectionCos& operator=(const DirectionCos &DCos);
public:
    double xCos_;
    double yCos_;
    double zCos_;
};

class CartesianPosition
{
public:
    CartesianPosition(double x = 0.0, double y = 0.0, double z = 0.0);
    CartesianPosition(const CartesianPosition &pos);
    CartesianPosition& operator=(const CartesianPosition &pos);
    CartesianPosition operator+(const CartesianPosition &pos) const;
    CartesianPosition& operator+=(const CartesianPosition &pos);
    double norm();
    static double distance(const CartesianPosition &srcPos, const CartesianPosition &dstPos);
    static CartesianPosition middlePosInLine(const CartesianPosition &src, const CartesianPosition &dst, double percent);
public:
    double x_;
    double y_;
    double z_;
};

class Vector3
{
public:
    Vector3(double x = 0.0, double y = 0.0, double z = 0.0);
    Vector3(const Vector3 &pos);
    Vector3& operator=(const Vector3 &pos);
    Vector3 operator+(const Vector3 &pos) const;
    Vector3& operator+=(const Vector3 &pos);
	Vector3 operator-(const Vector3 &pos) const;
	Vector3& operator-=(const Vector3 &pos);
        Vector3 operator/(const double &num) const;
	Vector3& operator/=(const double &num);
	Vector3 operator*(const Vector3 &pos) const;
    void normalise();
	double dotMultiply(const Vector3 &pos);

public:
    double x_;
    double y_;
    double z_;
};

class JointTheta
{
public:
    JointTheta(double theta1 = 0.0, double theta2 = 0.0, double theta3 = 0.0, double theta4 = 0.0, double theta5 = 0.0, double theta6 = 0.0);
    JointTheta(const JointTheta &pos);
    JointTheta& operator=(const JointTheta &pos);

    static double cartesianDistance(const JointTheta &srcThetas, const JointTheta &dstThetas);
    CartesianPosition cartesianPosition() const;
public:
    double theta1_;
    double theta2_;
    double theta3_;
    double theta4_;
    double theta5_;
    double theta6_;
};

#endif
