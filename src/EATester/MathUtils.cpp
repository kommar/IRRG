#include "MathUtils.h"

#include "RandUtils.h"

#include <cmath>

using namespace MathUtils;

double MathUtils::dComputeAngle(double *pdValues0, double *pdValues1, uint32_t iLength)
{
	double d_cosinus_value = dComputeDotProduct(pdValues0, pdValues1, iLength) / dComputeSecondNorm(pdValues0, iLength) / dComputeSecondNorm(pdValues1, iLength);
	
	if (d_cosinus_value > 1 && round(d_cosinus_value) == 1)
	{
		d_cosinus_value = 1;
	}//if (d_cosinus_value > 1 && round(d_cosinus_value) == 1)
	
	return acos(d_cosinus_value);
}//double MathUtils::dComputeAngle(double *pdValues0, double *pdValues1, uint32_t iLength)

double MathUtils::dComputeDotProduct(double *pdValues0, double *pdValues1, uint32_t iLength)
{
	double d_dot_product = 0;

	for (uint32_t i = 0; i < iLength; i++)
	{
		d_dot_product += *(pdValues0 + i) * *(pdValues1 + i);
	}//for (uint32_t i = 0; i < iLength; i++)

	return d_dot_product;
}//double MathUtils::dComputeDotProduct(double *pdValues0, double *pdValues1, uint32_t iLength)

double MathUtils::dComputeSecondNorm(double *pdValues, uint32_t iLength)
{
	return sqrt(dComputeDotProduct(pdValues, pdValues, iLength));
}//double MathUtils::dComputeSecondNorm(double *pdValues, uint32_t iLength)

double MathUtils::dComputeDistance(double *pdValues0, double *pdValues1, uint32_t iLength, double dMaxValue)
{
	return sqrt(dComputeSquareDistance(pdValues0, pdValues1, iLength, dMaxValue));
}//double MathUtils::dComputeDistance(double *pdValues0, double *pdValues1, uint32_t iLength, double dMaxValue)

double MathUtils::dComputeSquareDistance(double* pdValues0, double* pdValues1, uint32_t iLength, double dMaxValue)
{
	double d_square_distance = 0;

	for (uint32_t i = 0; i < iLength && d_square_distance < dMaxValue; i++)
	{
		d_square_distance += (*(pdValues0 + i) - *(pdValues1 + i)) * (*(pdValues0 + i) - *(pdValues1 + i));
	}//for (uint32_t i = 0; i < iLength && d_square_distance < dMaxValue; i++)

	if (d_square_distance > dMaxValue)
	{
		d_square_distance = dMaxValue;
	}//if (d_square_distance > dMaxValue)

	return d_square_distance;
}//double MathUtils::dComputeSquareDistance(double* pdValues0, double* pdValues1, uint32_t iLength, double dMaxValue)

double MathUtils::dComputeEntropy(uint32_t *piCounts, uint32_t iLength)
{
	uint32_t i_total = 0;

	for (uint32_t i = 0; i < iLength; i++)
	{
		i_total += *(piCounts + i);
	}//for (uint32_t i = 0; i < iLength; i++)

	return dComputeEntropy(piCounts, iLength, i_total);
}//double MathUtils::dComputeEntropy(uint32_t *piCounts, uint32_t iLength)

double MathUtils::dComputeEntropy(uint32_t *piCounts, uint32_t iLength, uint32_t iTotal)
{
	double d_entropy = 0;

	if (iTotal > 0)
	{
		double d_total = (double)iTotal;

		double d_probability;

		for (uint32_t i = 0; i < iLength; i++)
		{
			if (*(piCounts + i) > 0)
			{
				d_probability = *(piCounts + i) / d_total;
				d_entropy -= d_probability * log(d_probability);
			}//if (*(piCounts + i) > 0)
		}//for (uint32_t i = 0; i < iLength; i++)
	}//if (iTotal > 0)

	return d_entropy;
}//double MathUtils::dComputeEntropy(uint32_t *piCounts, uint32_t iLength, uint32_t iTotal)

double MathUtils::dMaxValue(double *pdValues, uint32_t iLength)
{
	double d_max_value = -DBL_MAX;

	for (uint32_t i = 0; i < iLength; i++)
	{
		if (*(pdValues + i) > d_max_value)
		{
			d_max_value = *(pdValues + i);
		}//if (*(pdValues + i) > d_max_value)
	}//for (uint32_t i = 0; i < iLength; i++)

	return d_max_value;
}//double MathUtils::dMaxValue(double *pdValues, uint32_t iLength)

double MathUtils::dSum(double *pdValues, uint32_t iLength)
{
	double d_sum = 0;

	for (uint32_t i = 0; i < iLength; i++)
	{
		d_sum += *(pdValues + i);
	}//for (uint32_t i = 0; i < iLength; i++)

	return d_sum;
}//double MathUtils::dSum(double *pdValues, uint32_t iLength)

double MathUtils::dMean(double *pdValues, uint32_t iLength)
{
	return dSum(pdValues, iLength) / (double)iLength;
}//double MathUtils::dMean(double *pdValues, uint32_t iLength)

double MathUtils::dComputeMonteCarloIntegral(function<double(double*)> fFunction, double dMaxFunctionValue, double *pdMinValues, double *pdMaxValues, uint8_t iNumberOfDimensions, uint32_t iNumberOfSamples)
{
	uint32_t i_number_of_hits = 0;

	double *pd_sample_values = new double[iNumberOfDimensions];

	for (uint32_t i = 0; i < iNumberOfSamples; i++)
	{
		for (uint8_t j = 0; j < iNumberOfDimensions; j++)
		{
			*(pd_sample_values + j) = RandUtils::dRandNumber(*(pdMinValues + j), *(pdMaxValues + j));
		}//for (uint8_t j = 0; j < iNumberOfDimensions; j++)

		if (RandUtils::dRandNumber(dMaxFunctionValue) <= fFunction(pd_sample_values))
		{
			i_number_of_hits++;
		}//if (RandUtils::dRandNumber(dMaxFunctionValue) <= fFunction(pd_sample_values))
	}//for (uint32_t i = 0; i < iNumberOfSamples; i++)

	delete pd_sample_values;

	double d_max_volume = dMaxFunctionValue;

	for (uint8_t i = 0; i < iNumberOfDimensions; i++)
	{
		d_max_volume *= *(pdMaxValues + i) - *(pdMinValues + i);
	}//for (uint8_t i = 0; i < iNumberOfDimensions; i++)

	return d_max_volume * (double)i_number_of_hits / (double)iNumberOfSamples;
}//double MathUtils::dComputeMonteCarloIntegral(function<double(double*)> fFunction, double dMaxFunctionValue, double *pdMinValues, double *pdMaxValues, uint8_t iNumberOfDimensions, uint32_t iNumberOfSamples)

double MathUtils::dComputeGaussianProbability(CVector2D *pvValues, CVector2D *pvMeans, CMatrix2D *pmCovariances)
{
	CMatrix2D m_inverted_covariances(*pmCovariances);
	m_inverted_covariances.cInvert();

	CVector2D v_values_subtracted_from_means(*pvValues);
	v_values_subtracted_from_means.cSubtract(*pvMeans);

	CVector2D v_copied_values_subtracted_from_means(v_values_subtracted_from_means);

	double d_probability = 1.0 / sqrt(DEF_TWO_PI * pmCovariances->dCalculateDeterminant());
	d_probability *= exp(-1.0 / 2.0 * v_copied_values_subtracted_from_means.cMultiply(m_inverted_covariances).dMultiply(v_values_subtracted_from_means));

	return d_probability;
}//double MathUtils::dComputeGaussianProbability(CVector2D *pvValues, CVector2D *pvMeans, CMatrix2D *pmCovariances)

void MathUtils::vNormalize(double *pdValues, uint32_t iLength)
{
	double d_second_norm = dComputeSecondNorm(pdValues, iLength);

	if (d_second_norm > 0)
	{
		for (uint32_t i = 0; i < iLength; i++)
		{
			*(pdValues + i) /= d_second_norm;
		}//for (uint32_t i = 0; i < iLength; i++)
	}//if (d_sum > 0)
}//void MathUtils::vNormalize(double *pdValues, uint32_t iLength)

double MathUtils::dNormalize(double dValue, double dMinValue, double dMaxValue)
{
	return (dValue - dMinValue) / (dMaxValue - dMinValue);
}//double MathUtils::dNormalize(double dValue, double dMinValue, double dMaxValue)

double MathUtils::dComputeGamma(double dMuM, double dN)
{
	return (dN * dMuM) / (1.0 - dN * dMuM);
}//double MathUtils::dComputeGamma(double dMuM, double dN)


CVector2D::CVector2D() : CVector2D(0, 0)
{

}//CVector2D::CVector2D() : CVector2D(0, 0)

CVector2D::CVector2D(double dValue0, double dValue1)
{
	v_set_values(dValue0, dValue1);
}//CVector2D::CVector2D(double dValue0, double dValue1)

CVector2D & CVector2D::cMultiply(CMatrix2D &cMatrix)
{
	double d_new_value_0 = d_value_0 * cMatrix.dGetValue00() + d_value_1 * cMatrix.dGetValue10();
	double d_new_value_1 = d_value_0 * cMatrix.dGetValue01() + d_value_1 * cMatrix.dGetValue11();

	v_set_values(d_new_value_0, d_new_value_1);

	return *this;
}//CVector2D & CVector2D::cMultiply(CMatrix2D &cMatrix)

CVector2D & CVector2D::cSubtract(CVector2D &cOther)
{
	d_value_0 -= cOther.d_value_0;
	d_value_1 -= cOther.d_value_1;

	return *this;
}//CVector2D & CVector2D::cSubtract(CVector2D &cOther)

double CVector2D::dMultiply(CVector2D &cOther)
{
	return d_value_0 * cOther.d_value_0 + d_value_1 * cOther.d_value_1;
}//double CVector2D::dMultiply(CVector2D &cOther)

void CVector2D::v_set_values(double dValue0, double dValue1)
{
	d_value_0 = dValue0;
	d_value_1 = dValue1;
}//void CVector2D::v_set_values(double dValue0, double dValue1)


CMatrix2D::CMatrix2D() : CMatrix2D(0, 0, 0, 0)
{

}//CMatrix2D::CMatrix2D() : CMatrix2D(0, 0, 0, 0)

CMatrix2D::CMatrix2D(double dValue00, double dValue01, double dValue10, double dValue11)
{
	v_set_values(dValue00, dValue01, dValue10, dValue11);
}//CMatrix2D::CMatrix2D(double dValue00, double dValue01, double dValue10, double dValue11)

CMatrix2D & CMatrix2D::cSubtract(CMatrix2D &cOther)
{
	d_value_00 -= cOther.d_value_00;
	d_value_01 -= cOther.d_value_01;
	d_value_10 -= cOther.d_value_10;
	d_value_11 -= cOther.d_value_11;

	return *this;
}//CMatrix2D & CMatrix2D::cSubtract(CMatrix2D &cOther)

CMatrix2D & CMatrix2D::cMultiply(CMatrix2D &cOther)
{
	double d_new_value_00 = d_value_00 * cOther.d_value_00 + d_value_01 * cOther.d_value_10;
	double d_new_value_01 = d_value_00 * cOther.d_value_01 + d_value_01 * cOther.d_value_11;
	double d_new_value_10 = d_value_10 * cOther.d_value_00 + d_value_11 * cOther.d_value_10;
	double d_new_value_11 = d_value_10 * cOther.d_value_01 + d_value_11 * cOther.d_value_11;

	v_set_values(d_new_value_00, d_new_value_01, d_new_value_10, d_new_value_11);

	return *this;
}//CMatrix2D & CMatrix2D::cMultiply(CMatrix2D &cOther)

CMatrix2D & CMatrix2D::cTranspose()
{
	double d_new_value_01 = d_value_10;
	double d_new_value_10 = d_value_01;

	v_set_values(d_value_00, d_new_value_01, d_new_value_10, d_value_11);

	return *this;
}//CMatrix2D & CMatrix2D::cTranspose()

CMatrix2D & CMatrix2D::cInvert()
{
	double d_new_value_00 = d_value_11;
	double d_new_value_01 = -d_value_01;
	double d_new_value_10 = -d_value_10;
	double d_new_value_11 = d_value_00;

	double d_determinant = dCalculateDeterminant();

	v_set_values(d_value_00, d_new_value_01, d_new_value_10, d_value_11);

	cDivide(d_determinant);

	return *this;
}//CMatrix2D & CMatrix2D::cInvert()

CMatrix2D & CMatrix2D::cDivide(double dValue)
{
	d_value_00 /= dValue;
	d_value_01 /= dValue;
	d_value_10 /= dValue;
	d_value_11 /= dValue;

	return *this;
}//CMatrix2D & CMatrix2D::cDivide(double dValue)

double CMatrix2D::dCalculateDeterminant()
{
	return d_value_00 * d_value_11 - d_value_01 * d_value_10;
}//double CMatrix2D::dCalculateDeterminant()

double CMatrix2D::dCalculateTrace()
{
	return d_value_00 + d_value_11;
}//double CMatrix2D::dCalculateTrace()

void CMatrix2D::vCalculateEigenvalues(double *pdEigenvalue0, double *pdEigenvalue1)
{
	double d_trace = dCalculateTrace();
	double d_determinant = dCalculateDeterminant();

	double d_factor_0 = d_trace / 2.0;
	double d_factor_1 = sqrt(d_trace * d_trace / 4.0 - d_determinant);

	*pdEigenvalue0 = d_factor_0 + d_factor_1;
	*pdEigenvalue1 = d_factor_0 - d_factor_1;
}//void CMatrix2D::vCalculateEigenvalues(double *pdEigenvalue0, double *pdEigenvalue1)

void CMatrix2D::v_set_values(double dValue00, double dValue01, double dValue10, double dValue11)
{
	d_value_00 = dValue00;
	d_value_01 = dValue01;
	d_value_10 = dValue10;
	d_value_11 = dValue11;
}//void CMatrix2D::v_set_values(double dValue00, double dValue01, double dValue10, double dValue11)