#include "StringCommandParam.h"

#include "IOUtils.h"

CStringCommandParam::CStringCommandParam(CString sName, bool bIsObligatory)
	: CCommandParam(sName, bIsObligatory, false)
{
	v_init(s_default_value);
}//CStringCommandParam::CStringCommandParam(CString sName, bool bIsObligatory)

CStringCommandParam::CStringCommandParam(CString sName, CString sDefaultValue)
	: CCommandParam(sName, false, true)
{
	v_init(sDefaultValue);
}//CStringCommandParam::CStringCommandParam(CString sName, CString sDefaultValue)

CString CStringCommandParam::sGetValue(istream *psSettings, CError *pcError)
{
	*pcError = eSetValue(psSettings);
	return s_value;
}//CString CStringCommandParam::sGetValue(istream *psSettings, CError *pcError)

CError CStringCommandParam::eSetValue(CString sValue)
{
	s_value = sValue;
	v_set_value();

	return CError();
}//CError CStringCommandParam::eSetValue(CString sValue)

void CStringCommandParam::v_init(CString sDefaultValue)
{
	s_default_value = sDefaultValue;
}//void CStringCommandParam::v_init(CString sDefaultValue)


CFilePathCommandParam::CFilePathCommandParam(CString sName, bool bIsObligatory)
	: CStringCommandParam(sName, bIsObligatory)
{

}//CFilePathCommandParam::CFilePathCommandParam(CString sName, bool bIsObligatory)

CFilePathCommandParam::CFilePathCommandParam(CString sName, CString sDefaultValue)
	: CStringCommandParam(sName, sDefaultValue)
{

}//CFilePathCommandParam::CFilePathCommandParam(CString sName, CString sDefaultValue)

#include <iostream>
using namespace std;

CError CFilePathCommandParam::eSetValue(CString sValue)
{
	CError c_error;

	if (IOUtils::bFileExists(sValue))
	{
		c_error = CStringCommandParam::eSetValue(sValue);
	}//if (IOUtils::bFileExists(sValue))
	else
	{
		c_error.vSetError(CError::iERROR_CODE_SYSTEM_WRONG_FILE_PATH, sValue);
	}//else if (IOUtils::bFileExists(sValue))

	return c_error;
}//CError CFilePathCommandParam::eSetValue(CString sValue)