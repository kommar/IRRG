#ifndef ENUM_COMMAND_PARAM_H
#define ENUM_COMMAND_PARAM_H

#include "CommandParam.h"
#include "Error.h"
#include "StringUtils.h"

#include <atlstr.h>
#include <unordered_map>

using namespace std;

template <typename TEnum>
class CEnumCommandParam : public CCommandParam
{
public:
	CEnumCommandParam(CString sName, unordered_map<CString, TEnum> *pmMappedValues, bool bIsObligatory = true);
	CEnumCommandParam(CString sName, unordered_map<CString, TEnum> *pmMappedValues, TEnum eDefaultValue);

	TEnum eGetValue() { return e_value; };
	TEnum eGetValue(istream *psSettings, CError *pcError);

	using CCommandParam::eSetValue;
	virtual CError eSetValue(CString sValue);
	virtual void vSetDefaultValue() { e_value = e_default_value; };

private:
	void v_init(unordered_map<CString, TEnum> *pmMappedValues, TEnum eDefaultValue);

	unordered_map<CString, TEnum> *pm_mapped_values;

	TEnum e_value;
	TEnum e_default_value;
};//class CEnumCommandParam

#endif//ENUM_COMMAND_PARAM_H