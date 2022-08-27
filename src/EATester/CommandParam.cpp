#include "CommandParam.h"

#include "Decomposition.h"
#include "Evaluation.h"
#include "RealCoding.h"

#include <cfloat>
#include <cstdlib>
#include <string>
#include <iostream>


CCommandParam::CCommandParam(CString sName, bool bIsObligatory, bool bHasDefaultValue)
{
	s_name = sName;
	b_is_obligatory = bIsObligatory;
	b_has_default_value = bHasDefaultValue;
	b_has_value_been_set = false;
}//CCommandParam::CCommandParam(CString sName, bool bIsObligatory, bool bHasDefaultValue)

CCommandParam::~CCommandParam()
{

}//CCommandParam::~CCommandParam()

CError CCommandParam::eSetValue(istream *psSettings)
{
	CError c_error;
	string s_line;
	
	streamoff i_settings_position_before_read = psSettings->tellg();

	if (getline(*psSettings, s_line))
	{
		CString s_cstring_line(s_line.c_str());

		CString s_param_value; 
		CString s_param_name; 

		int i_line_separator_position = s_cstring_line.Find("\\\\");

		if (i_line_separator_position != -1)
		{
			s_param_value = s_cstring_line.Mid(0, i_line_separator_position);
			s_param_name = s_cstring_line.Mid(i_line_separator_position + 2);
		}//if (i_line_separator_position != -1)

		if (s_name != s_param_name)
		{
			if (!bIsObligatory())
			{
				if (bHasDefaultValue())
				{
					vSetDefaultValue();
				}//if (bHasDefaultValue())

				psSettings->clear();
				psSettings->seekg(i_settings_position_before_read);
			}//if (!bIsObligatory())
			else
			{
				CString s_error_message;
				s_error_message.AppendFormat("line: %s; expected: %s; found: %s", s_cstring_line, s_name, s_param_name);

				c_error.vSetError(CError::iERROR_CODE_SYSTEM_MISMATCHING_ARGUMENT, s_error_message);
			}//else if (!bIsObligatory())
		}//if (s_name != s_param_name)
		else
		{
			c_error = eSetValue(s_param_value);

			if (c_error)
			{
				psSettings->clear();
				psSettings->seekg(i_settings_position_before_read);
			}//if (c_error)
		}//else if (s_name != s_param_name)
	}//if (getline(*psSettings, s_line))
	else
	{
		if (bIsObligatory())
		{
			c_error.vSetError(CError::iERROR_CODE_SYSTEM_SETTINGS_CANNOT_READ_LINE, s_name);
		}//if (bIsObligatory())
	}//else if (getline(*psSettings, s_line))

	return c_error;
}//CError CCommandParam::eSetValue(istream *psSettings)


CBoolCommandParam::CBoolCommandParam(CString sName, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{
	v_init(b_default_value);
}//CBoolCommandParam::CBoolCommandParam(CString sName, bool bIsObligatory)

CBoolCommandParam::CBoolCommandParam(CString sName, bool bDefaultValue, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, true)
{
	v_init(bDefaultValue);
}//CBoolCommandParam::CBoolCommandParam(CString sName, bool bDefaultValue, bool bIsObligatory)

bool CBoolCommandParam::bGetValue(istream *psSettings, CError *pcError)
{
	*pcError = eSetValue(psSettings);
	return b_value;
}//bool CBoolCommandParam::bGetValue(istream *psSettings, CError *pcError)

CError CBoolCommandParam::eSetValue(CString sValue)
{
	CError c_error;

	char *c_parsing_end_point;

	long l_value = strtol(sValue, &c_parsing_end_point, 10);

	if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)
	{
		if (l_value == 0 || l_value == 1)
		{
			b_value = (bool)l_value;
			v_set_value();
		}//if (l_value == 0 || l_value == 1)
		else
		{
			CString s_error_message;
			s_error_message.Format("%s must be 0 or 1", sGetName());
			
			c_error.vSetError(CError::iERROR_CODE_SYSTEM_OUT_OF_BOUND_ARGUMENT, s_error_message);
		}//else if (l_value == 0 || l_value == 1)
	}//if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)
	else
	{
		c_error.vSetError(CError::iERROR_CODE_SYSTEM_CANNOT_PARSE_ARGUMENT, sGetName());
	}//else if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)

	return c_error;
}//CError CBoolCommandParam::eSetValue(CString sValue)


CDoubleCommandParam::CDoubleCommandParam(CString sName, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{
	v_init(d_default_value);
}//CDoubleCommandParam::CDoubleCommandParam(CString sName, bool bIsObligatory)

CDoubleCommandParam::CDoubleCommandParam(CString sName, double dDefaultValue)
	: CCommandParam(sName, false, true)
{
	v_init(dDefaultValue);
}//CDoubleCommandParam::CDoubleCommandParam(CString sName, double dDefaultValue)

CDoubleCommandParam::CDoubleCommandParam(CString sName, double dMinValue, double dMaxValue, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{
	v_init(dMinValue, dMaxValue, d_default_value);
}//CDoubleCommandParam::CDoubleCommandParam(CString sName, double dMinValue, double dMaxValue, bool bIsObligatory)

CDoubleCommandParam::CDoubleCommandParam(CString sName, double dMinValue, double dMaxValue, double dDefaultValue)
	: CCommandParam(sName, false, true)
{
	v_init(dMinValue, dMaxValue, dDefaultValue);
}//CDoubleCommandParam::CDoubleCommandParam(CString sName, double dMinValue, double dMaxValue, double dDefaultValue)

double CDoubleCommandParam::dGetValue(istream * psSettings, CError *pcError)
{
	*pcError = CCommandParam::eSetValue(psSettings);
	return d_value;
}//double CDoubleCommandParam::dGetValue(istream * psSettings, CError *pcError)

CError CDoubleCommandParam::eSetValue(CString sValue)
{
	CError c_error;

	char *c_parsing_end_point;
	d_value = strtod(sValue, &c_parsing_end_point);

	if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)
	{
		if (d_value >= d_min_value && d_value <= d_max_value)
		{
			v_set_value();
		}//if (d_value >= d_min_value && d_value <= d_max_value)
		else
		{
			CString s_error_message;
			s_error_message.Format("%s [%f, %f]", sGetName(), d_min_value, d_max_value);

			c_error.vSetError(CError::iERROR_CODE_SYSTEM_OUT_OF_BOUND_ARGUMENT, s_error_message);
		}//else if (d_value >= d_min_value && d_value <= d_max_value)
	}//if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)
	else
	{
		c_error.vSetError(CError::iERROR_CODE_SYSTEM_CANNOT_PARSE_ARGUMENT, sGetName());
	}//else if (*c_parsing_end_point == '\0' && c_parsing_end_point != sValue)

	return c_error;
}//CError CDoubleCommandParam::eSetValue(CString sValue)

void CDoubleCommandParam::v_init(double dDefaultValue)
{
	v_init(-DBL_MAX, DBL_MAX, dDefaultValue);
}//void CDoubleCommandParam::v_init(double dDefaultValue)

void CDoubleCommandParam::v_init(double dMinValue, double dMaxValue, double dDefaultValue)
{
	d_min_value = dMinValue;
	d_max_value = dMaxValue;
	d_default_value = dDefaultValue;
}//void CDoubleCommandParam::v_init(double dMinValue, double dMaxValue, double dDefaultValue)


template <class TResult>
CConstructorCommandParam<TResult>::CConstructorCommandParam(CString sName, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{
	v_init(nullptr);
}//CConstructorCommandParam<TResult>::CConstructorCommandParam(CString sName, bool bIsObligatory)

template <class TResult>
CConstructorCommandParam<TResult>::CConstructorCommandParam(CString sName, TResult *pcDefaultValue)
	: CCommandParam(sName, false, true)
{
	v_init(pcDefaultValue);
}//CConstructorCommandParam<TResult>::CConstructorCommandParam(CString sName, TResult *pcDefaultValue)

template <class TResult>
void CConstructorCommandParam<TResult>::vAddConstructor(CString sConstructorName, function<TResult*()> fConstructor)
{
	m_mapped_constructors.insert(pair<const CString, function<TResult*()>>(sConstructorName, fConstructor));
}//void CConstructorCommandParam<TResult>::vAddConstructor(CString sConstructorName, function<TResult*()> fConstructor)

template <class TResult>
TResult * CConstructorCommandParam<TResult>::pcGetValue(istream *psSettings, CError *pcError)
{
	*pcError = eSetValue(psSettings);
	return pc_value;
}//TResult * CConstructorCommandParam<TResult>::pcGetValue(istream *psSettings, CError *pcError)

template <class TResult>
CError CConstructorCommandParam<TResult>::eSetValue(CString sValue)
{
	CError c_error;

	if (m_mapped_constructors.count(sValue) > 0)
	{
		pc_value = m_mapped_constructors.at(sValue)();
		v_set_value();
	}//if (m_mapped_constructors.count(sValue) > 0)
	else
	{
		CString s_error_message;
		s_error_message.AppendFormat("%s {", sGetName());

		unordered_map<CString, function<TResult*()>>::iterator i_iterator = m_mapped_constructors.begin();

		while (i_iterator != m_mapped_constructors.end())
		{
			s_error_message.Append(i_iterator->first);
			i_iterator++;

			if (i_iterator != m_mapped_constructors.end())
			{
				s_error_message.Append(", ");
			}//if (i_iterator != m_mapped_constructors.end())
		}//while (i_iterator != m_mapped_constructors.end())

		s_error_message.AppendChar('}');

		c_error.vSetError(CError::iERROR_CODE_SYSTEM_OUT_OF_BOUND_ARGUMENT, s_error_message);
	}//else if (m_mapped_constructors.count(sValue) > 0)

	return c_error;
}//CError CConstructorCommandParam<TResult>::eSetValue(CString sValue)


template class CConstructorCommandParam<CEvaluation<CRealCoding>>;

template class CConstructorCommandParam<Decomposition::CDecomposition<CRealCoding, CRealCoding>>;