#include "RealRandomGeneration.h"

#include "RandUtils.h"

CRealRandomGeneration::CRealRandomGeneration(uint16_t iNumberOfDimensions, double *pdMinValues, double *pdMaxValues, double dEqualEpsilon)
	: CRealGeneration(iNumberOfDimensions, pdMinValues, pdMaxValues, dEqualEpsilon)
{

}//CRealRandomGeneration::CRealRandomGeneration(uint16_t iNumberOfDimensions, double *pdMinValues, double *pdMaxValues, double dEqualEpsilon)

CRealRandomGeneration::CRealRandomGeneration(CRealCoding *pcSample)
	: CRealGeneration(pcSample)
{

}//CRealRandomGeneration::CRealRandomGeneration(CRealCoding *pcSample)

CRealCoding * CRealRandomGeneration::pcGenerate()
{
	double *pd_values = new double[i_number_of_dimensions];

	for (uint16_t i = 0; i < i_number_of_dimensions; i++)
	{
		*(pd_values + i) = RandUtils::dRandNumber(*(pd_min_values + i), *(pd_max_values + i));
	}//for (uint16_t i = 0; i < i_number_of_dimensions; i++)

	return new CRealCoding(i_number_of_dimensions, pd_values, pd_min_values, pd_max_values, d_equal_epsilon);
}//CRealCoding * CRealRandomGeneration::pcGenerate()