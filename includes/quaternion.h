#ifndef QUATERNION_H_H
#define QUATERNION_H_H
#include <math.h>
#include "matrix.h"
#include "transformmatrix.h"

#define Trace_Index_Num   4

//两个四元素夹角余弦值的零点及最大值
#define ZERO_COSA         0.0
#define MAX_COSA          1 - ROBOT_EPSILON
#define CLAMP(x , min , max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : x))

class EulerAngle
{
public:
    double yaw_ , pitch_, roll_;
public:
    EulerAngle(double roll = 0.0, double pitch = 0.0, double yaw = 0.0): yaw_(yaw) , pitch_(pitch) , roll_(roll) {}
    ~EulerAngle(void) {}
    EulerAngle operator+(const EulerAngle &ea) const
    {
        EulerAngle value;

        value.yaw_ = this->yaw_ + ea.yaw_;
        value.pitch_ = this->pitch_ + ea.pitch_;
        value.roll_ = this->roll_ + ea.roll_;

        return value;
    }

    static double distance(const EulerAngle &src, const EulerAngle &dst)
    {
        return sqrt(pow(src.roll_ - dst.roll_, 2.0) + pow(src.pitch_ - dst.pitch_, 2.0) + pow(src.yaw_ - dst.yaw_, 2.0));
    }

    static EulerAngle middleEaInRotation(const EulerAngle &src, const EulerAngle &dst, double percent)
    {
        EulerAngle middle;
        middle.yaw_ = src.yaw_ + (dst.yaw_ - src.yaw_)*percent;
        middle.pitch_ = src.pitch_ + (dst.pitch_ - src.pitch_)*percent;
        middle.roll_ = src.roll_ + (dst.roll_ - src.roll_)*percent;
        return middle;
    }
};

//四元素
class Quaternion
{
public:
    Quaternion(double x = 0.0, double y = 0.0, double z = 0.0, double w = 1.0,double theta4=0.0):x_(x), y_(y), z_(z), w_(w),theta4_(theta4){}
    Quaternion(const Quaternion &quat)
    {
        x_ = quat.x_;
        y_ = quat.y_;
        z_ = quat.z_;
        w_ = quat.w_;
		theta4_=quat.theta4_;
    }

    Quaternion& operator=(const Quaternion &quat)
    {
        x_ = quat.x_;
        y_ = quat.y_;
        z_ = quat.z_;
        w_ = quat.w_;
		theta4_=quat.theta4_;

        return *this;
    }

    Quaternion conjugate() const
    {
        return Quaternion(-x_, -y_, -z_, w_);
    }

    // 调用此函数必须检测四元数的模不为零
    Quaternion inverse() const
    {
        Quaternion inv;
        Quaternion conj = conjugate();
        double normSquare = pow(norm(), 2.0);

        inv.x_ = conj.x_ / normSquare;
        inv.y_ = conj.y_ / normSquare;
        inv.z_ = conj.z_ / normSquare;
        inv.w_ = conj.w_ / normSquare;
        
        return inv;
    }

        Quaternion exp(double t) const
	{
		Quaternion exp;
		if (fabs(w_) < 0.99999)
		{
			double  alpha = acos(w_);
			double newAlpha = t * alpha;
			exp.w_ = cos(newAlpha);
			double temp = sin(newAlpha) / sin(alpha);
			exp.x_ = x_*temp;
			exp.y_ = y_*temp;
			exp.z_ = z_*temp;
		}
		else
		{
			exp.x_ = x_;
			exp.y_ = y_;
			exp.z_ = z_;
			exp.w_ = w_;
		}

		return exp;
	}

    bool operator==(const Quaternion &quat) const
    {
        if (fabs(this->x_ - quat.x_) < 0.0000001
			&& fabs(this->y_ - quat.y_) < 0.0000001
			&& fabs(this->z_ - quat.z_) < 0.0000001
			&& fabs(this->w_ - quat.w_) < 0.0000001
			&& fabs(this->theta4_ - quat.theta4_) < 0.0000001)
        {
            return true;
        }
        return false;
    }

    Quaternion& operator += (const double & rhs)
    {
        w_ += rhs;    // exception guard
        return(*this);
    }

    Quaternion& operator += (const Quaternion &rhs)
    {
        x_ += rhs.x_;
        y_ += rhs.y_;
        z_ += rhs.z_;
        w_ += rhs.w_;
        theta4_+=rhs.theta4_;
        return(*this);
    }

    Quaternion operator +(const Quaternion &rhs)
    {
        return Quaternion(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_, w_ + rhs.w_,theta4_+rhs.theta4_);
    }

    Quaternion& operator -= (const double &rhs)
    {
        w_ -= rhs;
        return(*this);
    }

    Quaternion& operator -= (const Quaternion &rhs)
    {
        x_ -= rhs.x_;
        y_ -= rhs.y_;
        z_ -= rhs.z_;
        w_ -= rhs.w_;
		theta4_-=rhs.theta4_;

        return(*this);
    }

    Quaternion& operator *= (const Quaternion &rhs)
    {
        double xt = +w_*rhs.x_+x_*rhs.w_+y_*rhs.z_-z_*rhs.y_; 
        double yt = +w_*rhs.y_-x_*rhs.z_+y_*rhs.w_+z_*rhs.x_; 
        double zt = +w_*rhs.z_+x_*rhs.y_-y_*rhs.x_+z_*rhs.w_; 
        double wt = +w_*rhs.w_-x_*rhs.x_-y_*rhs.y_-z_*rhs.z_;

        x_ = xt;
        y_ = yt;
        z_ = zt;
        w_ = wt;
        return(*this);
    }

    Quaternion operator *(const double &rhs) const
    {
        return Quaternion(x_*rhs, y_*rhs, z_*rhs, w_*rhs);
    }

    Quaternion operator *(const Quaternion &rhs) const
    {
        return Quaternion(w_*rhs.x_+x_*rhs.w_+y_*rhs.z_-z_*rhs.y_,
                            w_*rhs.y_-x_*rhs.z_+y_*rhs.w_+z_*rhs.x_, 
                            w_*rhs.z_+x_*rhs.y_-y_*rhs.x_+z_*rhs.w_, 
                            w_*rhs.w_-x_*rhs.x_-y_*rhs.y_-z_*rhs.z_);
    }

    Vector3 operator* (const Vector3 &vec) const
    {
        Vector3 vn(vec);
        //vn.normalise();  //这里不需要标准化，否则生成向量会是在原向量上标准化的旋转

        Quaternion vecQuat, resQuat;
        vecQuat.x_ = vn.x_;
        vecQuat.y_ = vn.y_;
        vecQuat.z_ = vn.z_;
        vecQuat.w_ = 0.0;

        resQuat = vecQuat * conjugate();
        resQuat = *this * resQuat;

        return (Vector3(resQuat.x_, resQuat.y_, resQuat.z_));
    }

    void FromAxis(const Vector3 &v, double angle)
    {
        double sinAngle;
        angle *= 0.5;
        Vector3 vn(v);
        vn.normalise();

        sinAngle = sin(angle);

        x_ = (vn.x_ * sinAngle);
        y_ = (vn.y_ * sinAngle);
        z_ = (vn.z_ * sinAngle);
        w_ = cos(angle);
    }

    void fromEuler(const EulerAngle &ea)
    {
        double fCosHRoll = cos(ea.roll_ * .5f);
        double fSinHRoll = sin(ea.roll_ * .5f);
        double fCosHPitch = cos(ea.pitch_ * .5f);
        double fSinHPitch = sin(ea.pitch_ * .5f);
        double fCosHYaw = cos(ea.yaw_ * .5f);
        double fSinHYaw = sin(ea.yaw_ * .5f);

        /// Cartesian coordinate System
        w_ = fCosHRoll * fCosHPitch * fCosHYaw + fSinHRoll * fSinHPitch * fSinHYaw;
        x_ = fSinHRoll * fCosHPitch * fCosHYaw - fCosHRoll * fSinHPitch * fSinHYaw;
        y_ = fCosHRoll * fSinHPitch * fCosHYaw + fSinHRoll * fCosHPitch * fSinHYaw;
        z_ = fCosHRoll * fCosHPitch * fSinHYaw - fSinHRoll * fSinHPitch * fCosHYaw;

                double norm = sqrt(x_* x_ + y_* y_ + z_* z_ + w_* w_);
		x_ = x_ / norm;
		y_ = y_ / norm;
		z_ = z_ / norm;
		w_ = w_ / norm;
    }

  EulerAngle getEulerAngle() const
    {
        EulerAngle ea;
		double w, x, y, z;
		w = w_;
		x = x_;
		y = y_;
		z = z_;
		if (fabs(w_) < ROBOT_EPSILON)
		{
			w = 0;
		}

		if (fabs(x_) < ROBOT_EPSILON)
		{
			x = 0;
		}

		if (fabs(y_) < ROBOT_EPSILON)
		{
			y = 0;
		}

		if (fabs(z_) < ROBOT_EPSILON)
		{
			z = 0;
		}
        /// Cartesian coordinate System 
        ea.roll_  = atan2(2 * (w * x + y * z) , 1 - 2 * (x * x + y * y));
		ea.pitch_ = fabs(2 * (w * y - z * x)-1)<0.00001 ? (w * y - z * x) / fabs(w * y - z * x)*ROBOT_PI_DIVIDE_TWO : asin(2 * (w * y - z * x));
        ea.yaw_   = atan2(2 * (w * z + x * y) , 1 - 2 * (y * y + z * z));
        return ea;
     }

        void transQuaFromVectors(Vector3 dst, Vector3 src)
	{
		 dst.normalise();
		 src.normalise();

		 Vector3 temp = dst + src;
		 double norm = sqrt(temp.x_*temp.x_ + temp.y_*temp.y_ + temp.z_*temp.z_);
		 if (fabs(norm) < ROBOT_EPSILON)
		 {
			 w_ = 0;
			 x_ = 1;
			 y_ = 0;
			 z_ = 0;
		 }
		 else
		 {
			 Vector3 half;
			 half = (dst + src) / norm;
			 Vector3 multiply;
			 multiply = dst*half;
			 x_ = multiply.x_;
			 y_ = multiply.y_;
			 z_ = multiply.z_;
			 w_ = dst.dotMultiply(half);
		 }

	}


    static double distance(const Quaternion &src, const Quaternion &dst)
    {
        return EulerAngle::distance(src.getEulerAngle(), dst.getEulerAngle());
    }

    // 此函数只是计算原始姿态src与目标姿态dst同旋转轴的情况下
    // 他们之间的欧拉角的比例插值
    static Quaternion middleOrientInQuaternion(const Quaternion &src, const Quaternion &dst, double percent)
    {
        Quaternion quat;
        quat.fromEuler(EulerAngle::middleEaInRotation(src.getEulerAngle(), dst.getEulerAngle(), percent));
        return quat;
    }

    // 四元素长度等于(vec * vec)
    double norm() const
    {
        return  sqrt(x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
    }

    void normalise()
    {
        // Don't normalize if we don't have to
        double mag2 = w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_;
        if (!(mag2 < ROBOT_EPSILON && mag2 > -ROBOT_EPSILON)) 
        {
            double mag = sqrt(mag2);
            w_ /= mag;
            x_ /= mag;
            y_ /= mag;
            z_ /= mag;
        }
    }

    // 四元素长度等于(vec * vec)
    double length() const
    {
        return x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_;
    }

    double x_;
    double y_;
    double z_;
    double w_;
	double theta4_;

};

unsigned int homogeneousMatrixSlerp(RobotMatrix &m0, RobotMatrix &m1, double t, RobotMatrix &m);
unsigned int quatSlerp(RobotMatrix &m0, RobotMatrix &m1, double t, RobotMatrix& m);
unsigned int quaternionSlerp(Quaternion &q0, Quaternion &q1, double t,double dt, Quaternion& q, double& wx, double& wy, double& wz);
unsigned int Matrix2Quat(RobotMatrix matrix, Quaternion& q);
unsigned int TMatrix2Quat(RobotTMatrix Tmatrix, Quaternion& q);
unsigned int Quat2Matrix(Quaternion &q, RobotMatrix &matrix);
unsigned int Quat2EulerAngle(Quaternion &q, EulerAngle &ea);

#endif
