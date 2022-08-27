#include "UIntCommandParam.h"

#include <cstdlib>

CUIntCommandParam::CUIntCommandParam(CString sName, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{
	v_init(i_default_value);
}//CUIntCommandParam::CUIntCommandParam(CString sName, bool bIsObligatory)

CUIntCommandParam::CUIntCommandParam(CString sName, uint32_t iDefaultValue)
	: CCommandParam(sName, false, true)
{
	v_init(iDefaultValue);
}//CUIntCommandParam::CUIntCommandParam(CString sName, uint32_t iDefaultValue)

CUIntCommandParam::CUIntCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{
	v_init(iMinValue, iMaxValue, i_default_value);
}//CUIntCommandParam::CUIntCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, bool bIsObligatory)

CUIntCommandParam::CUIntCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, uint32_t iDefaultValue)
	: CCommandParam(sName, false, true)
{
	v_init(iMinValue, iMaxValue, iDefaultValue);
}//CUIntCommandParam::CUIntCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, uint32_t iDefaultValue)

uint32_t CUIntCommandParam::iGetValue(istream *psSettings, CError *pcError)
{
	*pcError = eSetValue(psSettings);
	return i_value;
}//uint32_t CUIntCommandParam::iGetValue(istream *psSettings, CError *pcError)

CError CUIntCommandParam::eSetValue(CString sValue)
{
	CError c_error;

	char *c_parsing_end_point;

	long l_value = strtol(sValue, &c_parsing_end_point, 10);

	if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)
	{
		if (l_value >= 0 && (uint32_t)l_value >= i_min_value && (uint32_t)l_value <= i_max_value)
		{
			i_value = (uint32_t)l_value;
			v_set_value();
		}//if (l_value >= i_min_value && l_value <= i_max_value)
		else
		{
			CString s_error_message;
			s_error_message.Format("%s [%u, %u]", sGetName(), i_min_value, i_max_value);
			
			c_error.vSetError(CError::iERROR_CODE_SYSTEM_OUT_OF_BOUND_ARGUMENT, s_error_message);
		}//else if (l_value >= i_min_value && l_value <= i_max_value)
	}//if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)
	else
	{
		c_error.vSetError(CError::iERROR_CODE_SYSTEM_CANNOT_PARSE_ARGUMENT, sGetName());
	}//else if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)

	return c_error;
}//CError CUIntCommandParam::eSetValue(CString sValue)

void CUIntCommandParam::v_init(uint32_t iDefaultValue)
{
	v_init(0, UINT32_MAX, iDefaultValue);
}//void CUIntCommandParam::v_init(uint32_t iDefaultValue)

void CUIntCommandParam::v_init(uint32_t iMinValue, uint32_t iMaxValue, uint32_t iDefaultValue)
{
	i_min_value = iMinValue;
	i_max_value = iMaxValue;
	i_default_value = iDefaultValue;
}//void CUIntCommandParam::v_init(uint32_t iMinValue, uint32_t iMaxValue, uint32_t iDefaultValue)