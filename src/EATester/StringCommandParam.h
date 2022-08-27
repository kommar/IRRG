#ifndef STRING_COMMAND_PARAM_H
#define STRING_COMMAND_PARAM_H

#include "CommandParam.h"
#include "Error.h"

#include <atlstr.h>


class CStringCommandParam : public CCommandParam
{
public:
	CStringCommandParam(CString sName, bool bIsObligatory = true);
	CStringCommandParam(CString sName, CString sDefaultValue);
	
	CString sGetValue() { return s_value; };
	CString sGetValue(istream *psSettings, CError *pcError);

	using CCommandParam::eSetValue;
	virtual CError eSetValue(CString sValue);
	virtual void vSetDefaultValue() { s_value = s_default_value; };

private:
	void v_init(CString sDefaultValue);

	CString s_value;
	CString s_default_value;
};//class CStringCommandParam : public CCommandParam


class CFilePathCommandParam : public CStringCommandParam
{
public:
	CFilePathCommandParam(CString sName, bool bIsObligatory = true);
	CFilePathCommandParam(CString sName, CString sDefaultValue);

	using CCommandParam::eSetValue;
	virtual CError eSetValue(CString sValue);
};//class CFilePathCommandParam : public CStringCommandParam

#endif//STRING_COMMAND_PARAM_H