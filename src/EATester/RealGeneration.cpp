#include "RealGeneration.h"

CRealGeneration::CRealGeneration(uint16_t iNumberOfDimensions, double *pdMinValues, double *pdMaxValues, double dEqualEpsilon)
	: CGeneration<CRealCoding>()
{
	i_number_of_dimensions = iNumberOfDimensions;

	pd_min_values = pdMinValues;
	pd_max_values = pdMaxValues;

	d_equal_epsilon = dEqualEpsilon;
}//CRealGeneration::CRealGeneration(uint16_t iNumberOfDimensions, double *pdMinValues, double *pdMaxValues, double dEqualEpsilon)

CRealGeneration::CRealGeneration(CRealCoding *pcSample)
	: CRealGeneration(pcSample->iGetNumberOfDimensions(), pcSample->pdGetMinValues(), pcSample->pdGetMaxValues(), pcSample->dGetEqualEpsilon())
{

}//CRealGeneration::CRealGeneration(CRealCoding *pcSample)