#ifndef ROBOT_MATRIX_H_H
#define ROBOT_MATRIX_H_H

#include "typedef.h"
#include "link.h"
#include "structure.h"

#define  MAX_MATRIX_SIZE 500

class RobotMatrix
{
public:
    RobotMatrix(unsigned int rowNum, unsigned int columnNum);
    RobotMatrix(const RobotMatrix &matrix);
    virtual ~RobotMatrix();

    ROBOT_BOOL operator==(const RobotMatrix &matrix) const;
    ROBOT_BOOL operator!=(const RobotMatrix &matrix) const ;
    RobotMatrix& operator=(const RobotMatrix &matrix);
    RobotMatrix operator+(const RobotMatrix &matrix) const;
    RobotMatrix& operator+=(const RobotMatrix &matrix);
    RobotMatrix operator-(const RobotMatrix &matrix) const;
    RobotMatrix& operator-=(const RobotMatrix &matrix);
    RobotMatrix operator*(const RobotMatrix &matrix) const;
    RobotMatrix& operator*=(const RobotMatrix &matrix);

	Vector3 operator+(const Vector3 &vector) const;
	RobotMatrix& operator+=(const Vector3 &vector);
	Vector3 operator*(const Vector3 &vector) const;
	RobotMatrix& operator*=(const Vector3 &vector);

    double& operator()(unsigned int row, unsigned int column);
    const double& operator()(unsigned int row, unsigned int column) const;

    void unit();
    void zero();
    void divide(double value);
    unsigned int getRowNum() const;
    unsigned int getColumnNum() const;
    RobotMatrix row(unsigned int rowIndex);
    RobotMatrix column(unsigned int columnIndex);

	int Rank(double &rank);
	int inverse(RobotMatrix &inverse_matrix);
	RobotMatrix rotation();
	int dotMultiplication(const RobotMatrix matrix, RobotMatrix &multiply);

    static const int ROBOT_T_DIMENSION = 4;

    static ROBOTStructure* structure_;
protected:
    double data_[MAX_MATRIX_SIZE];
    unsigned int rowNum_;
    unsigned int columnNum_;
};

#endif
