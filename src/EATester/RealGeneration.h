#ifndef REAL_GENERATION_H
#define REAL_GENERATION_H

#include "Generation.h"
#include "RealCoding.h"

#include <cstdint>

class CRealGeneration : public CGeneration<CRealCoding>
{
public:
	CRealGeneration(uint16_t iNumberOfDimensions, double *pdMinValues, double *pdMaxValues, double dEqualEpsilon);
	CRealGeneration(CRealCoding *pcSample);

	virtual CRealCoding *pcGenerate() { return pcGenerateEmpty(); };
	virtual CRealCoding *pcGenerateEmpty()
	{
		return new CRealCoding(i_number_of_dimensions, new double[i_number_of_dimensions], pd_min_values, pd_max_values, d_equal_epsilon);
	};

protected:
	uint16_t i_number_of_dimensions;

	double *pd_min_values;
	double *pd_max_values;

	double d_equal_epsilon;
};//class CRealGeneration

#endif//REAL_GENERATION_H