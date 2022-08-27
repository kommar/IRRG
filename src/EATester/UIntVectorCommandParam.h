#ifndef U_INT_VECTOR_COMMAND_PARAM_H
#define U_INT_VECTOR_COMMAND_PARAM_H

#include "CommandParam.h"
#include "Error.h"
#include "UIntCommandParam.h"

#include <atlstr.h>
#include <cstdint>
#include <vector>

using namespace std;

class CUIntVectorCommandParam : public CCommandParam
{
public:
	CUIntVectorCommandParam(CString sName, uint32_t iExpectedNumberOfValues, bool bIsObligatory = true);
	CUIntVectorCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, uint32_t iExpectedNumberOfValues, bool bIsObligatory = true);

	using CCommandParam::eSetValue;
	virtual CError eSetValue(CString sValue);
	virtual void vSetDefaultValue() { };

	vector<uint32_t> *pvGetValues() { return &v_values; };

private:
	uint32_t i_expected_number_of_values;

	vector<uint32_t> v_values;
	CUIntCommandParam c_u_int_command_param;
};//class CUIntVectorCommandParam

#endif//U_INT_VECTOR_COMMAND_PARAM_H