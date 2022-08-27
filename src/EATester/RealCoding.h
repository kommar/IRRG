#ifndef REAL_CODING_H
#define REAL_CODING_H

#include "GenePattern.h"

#include <atlstr.h>
#include <cstdint>
#include <ostream>

using namespace std;

class CRealCoding
{
public:
	CRealCoding(uint16_t iNumberOfDimensions, double *pdValues, double *pdMinValues, double *pdMaxValues, double dEqualEpsilon);
	CRealCoding(CRealCoding *pcOther);
	
	~CRealCoding();

	void vMerge(CRealCoding *pcOther);

	void vRepair();

	double dComputeDistance(CRealCoding *pcOther);
	double dComputeDistance(CRealCoding *pcOther, CGenePattern *pcGenePattern);

	bool bExceededMaxDistance(CRealCoding *pcOther, double dMaxDistance, double *pdDistance);
	bool bExceededMaxDistance(CRealCoding *pcOther, CGenePattern *pcGenePattern, double dMaxDistance, double *pdDistance, bool bOnlyPattern);

	bool bHasSameParams(CRealCoding *pcOther);

	uint16_t iGetNumberOfDimensions() { return i_number_of_dimensions; };

	double *pdGetValues() { return pd_values; };

	double *pdGetMinValues() { return pd_min_values; };
	double *pdGetMaxValues() { return pd_max_values; };

	double dGetEqualEpsilon() { return d_equal_epsilon; };

	void vSetValues(double *pdValues, bool bOwnValues = false);
	void vSetEmptyValues() { vSetValues(new double[i_number_of_dimensions], true); }

	void vSetMinMaxValues(double *pdMinValues, double *pdMaxValues, bool bOwnMinMaxValues = false);

	CString sToString();

	bool operator==(CRealCoding &cOther);
	bool operator!=(CRealCoding &cOther);

	friend ostream& operator<<(ostream &sOutput, CRealCoding *pcRealCoding);

private:
	bool b_own_values;
	bool b_own_min_max_values;

	uint16_t i_number_of_dimensions;

	double *pd_values;

	double *pd_min_values;
	double *pd_max_values;

	double d_equal_epsilon;
};//class CRealCoding

#endif//REAL_CODING_H