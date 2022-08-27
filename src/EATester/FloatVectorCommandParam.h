#ifndef FLOAT_VECTOR_COMMAND_PARAM_H
#define FLOAT_VECTOR_COMMAND_PARAM_H

#include "CommandParam.h"
#include "Error.h"
#include "FloatCommandParam.h"

#include <atlstr.h>
#include <cstdint>
#include <istream>
#include <vector>

using namespace std;

class CFloatVectorCommandParam : public CCommandParam
{
public:
	CFloatVectorCommandParam(CString sName, uint32_t iExpectedNumberOfValues, bool bIsObligatory = true);
	CFloatVectorCommandParam(CString sName, float fMinValue, float fMaxValue, uint32_t iExpectedNumberOfValues, bool bIsObligatory = true);

	using CCommandParam::eSetValue;
	virtual CError eSetValue(CString sValue);
	virtual void vSetDefaultValue() { };

	void vGetValues(float **ppfValues, istream *psSettings, CError *pcError);
	void vGetValues(double **ppdValues, istream *psSettings, CError *pcError);

private:
	uint32_t i_expected_number_of_values;

	vector<float> v_values;
	CFloatCommandParam c_float_command_param;
};//class CFloatVectorCommandParam : public CCommandParam

#endif//FLOAT_VECTOR_COMMAND_PARAM_H