#include "UIntVectorCommandParam.h"

#include "util\tools.h"

CUIntVectorCommandParam::CUIntVectorCommandParam(CString sName, uint32_t iExpectedNumberOfValues, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false), c_u_int_command_param(sName, 0, UINT32_MAX)
{
	i_expected_number_of_values = iExpectedNumberOfValues;
}//CUIntVectorCommandParam::CUIntVectorCommandParam(CString sName, uint32_t iExpectedNumberOfValues, bool bIsObligatory)

CUIntVectorCommandParam::CUIntVectorCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, uint32_t iExpectedNumberOfValues, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false), c_u_int_command_param(sName, iMinValue, iMaxValue)
{
	i_expected_number_of_values = iExpectedNumberOfValues;
}//CUIntVectorCommandParam::CUIntVectorCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, uint32_t iExpectedNumberOfValues, bool bIsObligatory)

CError CUIntVectorCommandParam::eSetValue(CString sValue)
{
	CError c_error;

	v_values.clear();

	int i_index = 0;
	CString s_single_value;

	while (!c_error && i_index < sValue.GetLength())
	{
		s_single_value = Tools::sExtractFromString(sValue, i_index, &i_index);
		c_error = c_u_int_command_param.eSetValue(s_single_value);

		if (!c_error)
		{
			v_values.push_back(c_u_int_command_param.iGetValue());
		}//if (!c_error)
	}//while (!c_error && i_index < sValue.GetLength())

	if (!c_error)
	{
		if (i_expected_number_of_values != (uint32_t)v_values.size())
		{
			CString s_error_message;
			s_error_message.Format("expected: %u; added: %u", i_expected_number_of_values, (uint32_t)v_values.size());

			c_error.vSetError(CError::iERROR_CODE_SYSTEM_ARGUMENT_WRONG_NUMBER_OF_VALUES, s_error_message);
		}//if (i_expected_number_of_values != (uint32_t)v_values.size())
		else
		{
			v_set_value();
		}//else if (i_expected_number_of_values != (uint32_t)v_values.size())
	}//if (!c_error)

	return c_error;
}//CError CUIntVectorCommandParam::eSetValue(CString sValue)