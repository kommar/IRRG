#include "FloatVectorCommandParam.h"

#include "util\tools.h"

CFloatVectorCommandParam::CFloatVectorCommandParam(CString sName, uint32_t iExpectedNumberOfValues, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false), c_float_command_param(sName)
{
	i_expected_number_of_values = iExpectedNumberOfValues;
}//CFloatVectorCommandParam::CFloatVectorCommandParam(CString sName, uint32_t iExpectedNumberOfValues, bool bIsObligatory)

CFloatVectorCommandParam::CFloatVectorCommandParam(CString sName, float fMinValue, float fMaxValue, uint32_t iExpectedNumberOfValues, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false), c_float_command_param(sName, fMinValue, fMaxValue)
{
	i_expected_number_of_values = iExpectedNumberOfValues;
}//CFloatVectorCommandParam::CFloatVectorCommandParam(CString sName, uint32_t iMinValue, uint32_t iMaxValue, uint32_t iExpectedNumberOfValues, bool bIsObligatory)

#include <iostream>
using namespace std;

CError CFloatVectorCommandParam::eSetValue(CString sValue)
{
	CError c_error;

	v_values.clear();

	int i_index = 0;
	CString s_single_value;

	while (!c_error && i_index < sValue.GetLength())
	{
		s_single_value = Tools::sExtractFromString(sValue, i_index, &i_index);
		c_error = c_float_command_param.eSetValue(s_single_value);

		if (!c_error)
		{
			v_values.push_back(c_float_command_param.fGetValue());
		}//if (!c_error)
	}//while (!c_error && i_index < sValue.GetLength())

	if (!c_error)
	{
		cout << "oj" << endl;

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

	cout << !c_error << endl;

	return c_error;
}//CError CFloatVectorCommandParam::eSetValue(CString sValue)

void CFloatVectorCommandParam::vGetValues(float **ppfValues, istream *psSettings, CError *pcError)
{
	*pcError = CCommandParam::eSetValue(psSettings);

	if (!*pcError)
	{
		*ppfValues = new float[i_expected_number_of_values];

		for (uint32_t i = 0; i < i_expected_number_of_values; i++)
		{
			*(*ppfValues + i) = v_values.at(i);
		}//for (uint32_t i = 0; i < i_expected_number_of_values; i++)
	}//if (!*pcError)
}//void CFloatVectorCommandParam::vGetValues(float **ppfValues, istream *psSettings, CError *pcError)

void CFloatVectorCommandParam::vGetValues(double **ppdValues, istream *psSettings, CError *pcError)
{
	*pcError = CCommandParam::eSetValue(psSettings);

	if (!*pcError)
	{
		cout << "hi" << endl;

		*ppdValues = new double[i_expected_number_of_values];

		for (uint32_t i = 0; i < i_expected_number_of_values; i++)
		{
			*(*ppdValues + i) = (double)v_values.at(i);
		}//for (uint32_t i = 0; i < i_expected_number_of_values; i++)
	}//if (!*pcError)

	cout << pcError->iGetCode() << endl;
}//void CFloatVectorCommandParam::vGetValues(double **ppdValues, istream *psSettings, CError *pcError)