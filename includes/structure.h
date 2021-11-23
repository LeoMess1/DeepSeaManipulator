#ifndef ROBOT_STRUCTURE_H_H
#define ROBOT_STRUCTURE_H_H

#include "typedef.h"
#include "link.h"
#include "jointtheta.h"

#define ROBOTSTRUCTURE ROBOTStructure::Instance()

class ROBOTStructure
{
public:
    static ROBOTStructure *Instance();

    const ROBOTLink* getLink(unsigned int linkIndex) ;
    void setLinkParam(unsigned int linkIndex, double a, double alpha, double d) ;
    void setThetaRange(unsigned int linkIndex, double max, double min) ;
    void setTheta(double theta1, double theta2, double theta3, double theta4, double theta5, double theta6) ;
    void setTheta(unsigned int thetaIndex, double theta);
    void setTheta(const double *theta);
    void setTheta(const JointTheta &thetas);
	void setDynamicsParam(unsigned int linkIndex, double mx, double my, double mz, double m);
	void setSpeed(const double *speed);
	void setAcc(const double *acc);
    double& getSinAlpha(unsigned int jointIndex) ;
    double& getCosAlpha(unsigned int jointIndex) ;
    double& getSinTheta(unsigned int jointIndex) ;
    double& getCosTheta(unsigned int jointIndex) ;
    const double& getTheta(unsigned int jointIndex) ;
    const double& getThetaVelocity(unsigned int jointIndex) ;
    const double& getThetaMax(unsigned int jointIndex) ;
    const double& getThetaMin(unsigned int jointIndex) ;
	const double& getMx(unsigned int jointIndex);
	const double& getMy(unsigned int jointIndex);
	const double& getMz(unsigned int jointIndex);
	const double& getM(unsigned int jointIndex);
	double* getSpeed(){ return &speed_[0]; };
	double* getAcc(){ return &acc_[0]; };
private:
	ROBOTLink       link_[ROBOT_JOINT_NUMBER + 1];
	double  thetaVelocity_[ROBOT_JOINT_NUMBER + 1];
	double  linkSinAlpha_[ROBOT_JOINT_NUMBER + 1];
	double  linkCosAlpha_[ROBOT_JOINT_NUMBER + 1];
	double  linkSinTheta_[ROBOT_JOINT_NUMBER + 1];
	double  linkCosTheta_[ROBOT_JOINT_NUMBER + 1];
	double  speed_[ROBOT_JOINT_NUMBER];
	double  acc_[ROBOT_JOINT_NUMBER];

private:
    ROBOTStructure() ;
    ROBOTStructure(const ROBOTStructure&){}
    static ROBOTStructure *instance_;
};

#endif
