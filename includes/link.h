#ifndef ROBOT_LINK_H_H
#define ROBOT_LINK_H_H

class ROBOTLink
{
public:
    ROBOTLink();
    ROBOTLink(double a, double alpha, double d, double theta);

    void  setA(double a)  {a_ = a;}
    void  setAlpha(double alpha)  {alpha_ = alpha;}
    void  setD(double d)  {d_ = d;}
    void  setTheta(double theta)  {theta_ = theta;}
    void  setThetaMax(double thetaMax)  {thetaMax_ = thetaMax;}
    void  setThetaMin(double thetaMin)  {thetaMin_ = thetaMin;}
	void  setMx(double mx){ mx_ = mx; }
	void  setMy(double my){ my_ = my; }
	void  setMz(double mz){ mz_ = mz; }
	void  setM(double m){ m_ = m; }

    const double& getA() const {return a_;}
    const double& getAlpha() const  {return alpha_;}
    const double& getD() const  {return d_;}
    const double& getTheta() const  {return theta_;}
    const double& getThetaMax() const  {return thetaMax_;}
    const double& getThetaMin() const  {return thetaMin_;}
	const double& getMx() const { return mx_; }
	const double& getMy() const { return my_; }
	const double& getMz() const { return mz_; }
	const double& getM() const { return m_; }
private:
    double        a_;
    double        alpha_;
    double        d_;
    double        theta_;

	double        mx_;
	double        my_;
	double        mz_;
	double        m_;

    double        thetaMax_;
    double        thetaMin_;
};

#endif //ROBOT_LINK_H_H
