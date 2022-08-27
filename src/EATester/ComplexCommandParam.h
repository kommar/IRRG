#ifndef COMPLEX_COMMAND_PARAM_H
#define COMPLEX_COMMAND_PARAM_H

#include "CommandParam.h"
#include "Error.h"

#include <atlstr.h>
#include <vector>

using namespace std;

class CComplexCommandParam : public CCommandParam
{
public:
	CComplexCommandParam(CString sName, bool bIsObligatory = true);

	using CCommandParam::eSetValue;
	virtual CError eSetValue(CString sValue);
	virtual void vSetDefaultValue() { };

	void vAddParam(CCommandParam* pcParam) { v_params.push_back(pcParam); };

private:
	vector<CCommandParam*> v_params;
};

#endif//COMPLEX_COMMAND_PARAM_H