#ifndef U_INT_COMMAND_PARAM_H
#define U_INT_COMMAND_PARAM_H

#include "CommandParam.h"
#include "Error.h"

#include <atlstr.h>
#include <cstdint>
#include <istream>

using namespace std;

class CUIntCommandParam : public CCommandParam
{
public:
	CUIntCommandParam(CString sName, bool bIsObligatory = true);
	CUIntCommandParam(CString sName, uint32_t iDefaultValue);
	CUIntCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, bool bIsObligatory = true);
	CUIntCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, uint32_t iDefaultValue);

	uint32_t iGetValue() { return i_value; };
	uint32_t iGetValue(istream *psSettings, CError *pcError);

	using CCommandParam::eSetValue;
	virtual CError eSetValue(CString sValue);
	virtual void vSetDefaultValue() { i_value = i_default_value; };

private:
	void v_init(uint32_t iDefaultValue);
	void v_init(uint32_t iMinValue, uint32_t iMaxValue, uint32_t iDefaultValue);

	uint32_t i_value;
	uint32_t i_default_value;

	uint32_t i_min_value;
	uint32_t i_max_value;
};//class CUIntCommandParam

#endif//U_INT_COMMAND_PARAM_H