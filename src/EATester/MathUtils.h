#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#ifndef DEF_TWO_PI
#define DEF_TWO_PI 2.0 * 3.14159265
#endif//DEF_TWO_PI

#include <cfloat>
#include <cstdint>
#include <functional>

using namespace std;

namespace MathUtils
{
	class CVector2D;
	class CMatrix2D;

	double dComputeAngle(double *pdValues0, double *pdValues1, uint32_t iLength);
	double dComputeDotProduct(double *pdValues0, double *pdValues1, uint32_t iLength);
	double dComputeSecondNorm(double *pdValues, uint32_t iLength);
	double dComputeDistance(double *pdValues0, double *pdValues1, uint32_t iLength, double dMaxValue = DBL_MAX);
	double dComputeSquareDistance(double *pdValues0, double *pdValues1, uint32_t iLength, double dMaxValue = DBL_MAX);

	double dComputeEntropy(uint32_t *piCounts, uint32_t iLength);
	double dComputeEntropy(uint32_t *piCounts, uint32_t iLength, uint32_t iTotal);

	double dMaxValue(double *pdValues, uint32_t iLength);
	double dSum(double *pdValues, uint32_t iLength);
	double dMean(double *pdValues, uint32_t iLength);

	double dComputeMonteCarloIntegral(function<double(double*)> fFunction, double dMaxFunctionValue, double *pdMinValues, 
		double *pdMaxValues, uint8_t iNumberOfDimensions, uint32_t iNumberOfSamples);

	double dComputeGaussianProbability(CVector2D *pvValues, CVector2D *pvMeans, CMatrix2D *pmCovariances);

	void vNormalize(double *pcValues, uint32_t iLength);
	double dNormalize(double dValue, double dMinValue, double dMaxValue);

	double dComputeGamma(double dMuM, double dN);


	class CVector2D
	{
	public:
		CVector2D();
		CVector2D(double dValue0, double dValue1);

		CVector2D& cSubtract(CVector2D& cOther);
		double dMultiply(CVector2D& cOther);
		
		CVector2D& cMultiply(CMatrix2D& cMatrix);

		double dGetValue0() { return d_value_0; }
		double dGetValue1() { return d_value_1; }

	private:
		void v_set_values(double dValue0, double dValue1);

		double d_value_0;
		double d_value_1;
	};//class CVector2D


	class CMatrix2D
	{
	public:
		CMatrix2D();
		CMatrix2D(double dValue00, double dValue01, double dValue10, double dValue11);

		CMatrix2D& cSubtract(CMatrix2D& cOther);
		CMatrix2D& cMultiply(CMatrix2D& cOther);

		CMatrix2D& cTranspose();
		CMatrix2D& cInvert();

		CMatrix2D& cDivide(double dValue);

		double dCalculateDeterminant();
		double dCalculateTrace();

		void vCalculateEigenvalues(double *pdEigenvalue0, double *pdEigenvalue1);

		double dGetValue00() { return d_value_00; }
		double dGetValue01() { return d_value_01; }
		double dGetValue10() { return d_value_10; }
		double dGetValue11() { return d_value_11; }

	private:
		void v_set_values(double dValue00, double dValue01, double dValue10, double dValue11);

		double d_value_00;
		double d_value_01;
		double d_value_10;
		double d_value_11;
	};//class CMatrix2D
}//namespace MathUtils

#endif//MATH_UTILS_H