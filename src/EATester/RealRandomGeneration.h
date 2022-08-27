#ifndef REAL_RANDOM_GENERATION_H
#define REAL_RANDOM_GENERATION_H

#include "RealCoding.h"
#include "RealGeneration.h"

#include <cstdint>

using namespace std;

class CRealRandomGeneration : public CRealGeneration
{
public:
	CRealRandomGeneration(uint16_t iNumberOfDimensions, double *pdMinValues, double *pdMaxValues, double dEqualEpsilon);
	CRealRandomGeneration(CRealCoding *pcSample);

	virtual CRealCoding *pcGenerate();
};//class CRealRandomGeneration

#endif//REAL_RANDOM_GENERATION_H