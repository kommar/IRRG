#ifndef COMMAND_PARAM_PARSER_H
#define COMMAND_PARAM_PARSER_H

#include "CommandParam.h"
#include "Error.h"
#include "StringUtils.h"

#include <atlstr.h>
#include <unordered_map>
#include <vector>

using namespace std;

class CCommandParamParser
{
public:
	CCommandParamParser(unordered_map<CString, CString> *pmArguments);

	~CCommandParamParser();

	void vRegister(CCommandParam *pcParam);

	CError eParse();

private:
	unordered_map<CString, CString> *pm_arguments;
	vector<CCommandParam*> v_params;
};//class CCommandParamParser

#endif//COMMAND_PARAM_PARSER_H