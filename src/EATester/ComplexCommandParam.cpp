#include "ComplexCommandParam.h"

#include "util\tools.h"

#include <cstdint>

CComplexCommandParam::CComplexCommandParam(CString sName, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{

}//CComplexCommandParam::CComplexCommandParam(CString sName, bool bIsObligatory)

CError CComplexCommandParam::eSetValue(CString sValue)
{
	CError c_error;

	uint32_t i_param_index;

	int i_extracting_string_index = 0;
	CString s_single_value;

	while (!c_error && i_extracting_string_index < sValue.GetLength() && i_param_index < (uint32_t)v_params.size())
	{
		s_single_value = Tools::sExtractFromString(sValue, i_extracting_string_index, &i_extracting_string_index);
		c_error = v_params.at(i_param_index)->eSetValue(s_single_value);

		i_param_index++;
	}//while (!c_error && i_extracting_string_index < sValue.GetLength() && i_param_index < (uint32_t)v_params.size())

	if (!c_error)
	{
		if (i_param_index < (uint32_t)v_params.size())
		{
			CString s_error_message;
			s_error_message.Format("expected: %u; added: %u", (uint32_t)v_params.size(), i_param_index);

			c_error.vSetError(CError::iERROR_CODE_SYSTEM_ARGUMENT_WRONG_NUMBER_OF_VALUES, s_error_message);
		}//if (i_param_index < (uint32_t)v_params.size())
		else if (i_extracting_string_index < sValue.GetLength())
		{
			CString s_error_message;
			s_error_message.Format("expected: %u; found: more", (uint32_t)v_params.size());

			c_error.vSetError(CError::iERROR_CODE_SYSTEM_ARGUMENT_WRONG_NUMBER_OF_VALUES, s_error_message);
		}//else if (i_extracting_string_index < sValue.GetLength()) 
	}//if (!c_error)

	return c_error;
}//CError CComplexCommandParam::eSetValue(CString sValue)