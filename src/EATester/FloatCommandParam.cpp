#include "FloatCommandParam.h"

#include <cfloat>
#include <cstdlib>

CFloatCommandParam::CFloatCommandParam(CString sName, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{
	v_init(f_default_value);
}//CFloatCommandParam::CFloatCommandParam(CString sName, bool bIsObligatory)

CFloatCommandParam::CFloatCommandParam(CString sName, float fDefaultValue)
	: CCommandParam(sName, false, true)
{
	v_init(fDefaultValue);
}//CFloatCommandParam::CFloatCommandParam(CString sName, float fDefaultValue)

CFloatCommandParam::CFloatCommandParam(CString sName, float fMinValue, float fMaxValue, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{
	v_init(fMinValue, fMaxValue, f_default_value);
}//CFloatCommandParam::CFloatCommandParam(CString sName, float fMinValue, float fMaxValue, bool bIsObligatory)

CFloatCommandParam::CFloatCommandParam(CString sName, float fMinValue, float fMaxValue, float fDefaultValue)
	: CCommandParam(sName, false, true)
{
	v_init(fMinValue, fMaxValue, fDefaultValue);
}//CFloatCommandParam::CFloatCommandParam(CString sName, float fMinValue, float fMaxValue, float fDefaultValue)

float CFloatCommandParam::fGetValue(istream *psSettings, CError *pcError)
{
	*pcError = CCommandParam::eSetValue(psSettings);
	return f_value;
}//float CFloatCommandParam::iGetValue(istream *psSettings, CError *pcError)

CError CFloatCommandParam::eSetValue(CString sValue)
{
	CError c_error;

	char *c_parsing_end_point;
	double d_value = strtod(sValue, &c_parsing_end_point);

	if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)
	{
		if (d_value >= f_min_value && d_value <= f_max_value)
		{
			f_value = (float)d_value;
			v_set_value();
		}//if (f_value >= f_min_value && f_value <= f_max_value)
		else
		{
			CString s_error_message;
			s_error_message.Format("%s [%f, %f]", sGetName(), f_min_value, f_max_value);
			
			c_error.vSetError(CError::iERROR_CODE_SYSTEM_OUT_OF_BOUND_ARGUMENT, s_error_message);
		}//else if (d_value >= f_min_value && d_value <= f_max_value)
	}//if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)
	else
	{
		c_error.vSetError(CError::iERROR_CODE_SYSTEM_CANNOT_PARSE_ARGUMENT, sGetName());
	}//else if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)

	return c_error;
}//CError CFloatCommandParam::eSetValue(CString sValue)

void CFloatCommandParam::v_init(float fDefaultValue)
{
	v_init(-FLT_MAX, FLT_MAX, fDefaultValue);
}//void CFloatCommandParam::v_init(float fDefaultValue)

void CFloatCommandParam::v_init(float fMinValue, float fMaxValue, float fDefaultValue)
{
	f_min_value = fMinValue;
	f_max_value = fMaxValue;
	f_default_value = fDefaultValue;
}//void CFloatCommandParam::v_init(float fMinValue, float fMaxValue, float fDefaultValue)