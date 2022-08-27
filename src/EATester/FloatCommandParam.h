#ifndef FLOAT_COMMAND_PARAM_H
#define FLOAT_COMMAND_PARAM_H

#include "CommandParam.h"
#include "Error.h"

#include <atlstr.h>
#include <istream>

using namespace std;

class CFloatCommandParam : public CCommandParam
{
public:
	CFloatCommandParam(CString sName, bool bIsObligatory = true);
	CFloatCommandParam(CString sName, float fDefaultValue);
	CFloatCommandParam(CString sName, float fMinValue, float fMaxValue, bool bIsObligatory = true);
	CFloatCommandParam(CString sName, float fMinValue, float fMaxValue, float fDefaultValue);

	float fGetValue() { return f_value; };
	float fGetValue(istream *psSettings, CError *pcError);

	using CCommandParam::eSetValue;
	virtual CError eSetValue(CString sValue);
	virtual void vSetDefaultValue() { f_value = f_default_value; };

private:
	void v_init(float fDefaultValue);
	void v_init(float fMinValue, float fMaxValue, float fDefaultValue);

	float f_value;
	float f_default_value;

	float f_min_value;
	float f_max_value;
};//class CFloatCommandParam

#endif//FLOAT_COMMAND_PARAM_H