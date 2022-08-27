#include "CommandParamParser.h"

#include <cstdint>

CCommandParamParser::CCommandParamParser(unordered_map<CString, CString> *pmArguments)
{
	pm_arguments = pmArguments;
}//CCommandParamParser::CCommandParamParser(unordered_map<CString, CString> *pmArguments)

CCommandParamParser::~CCommandParamParser()
{
	v_params.clear();
}//CCommandParamParser::~CCommandParamParser()

void CCommandParamParser::vRegister(CCommandParam *pcParam)
{
	v_params.push_back(pcParam);
}//void CCommandParamParser::vRegister(CCommandParam *pcParam)

CError CCommandParamParser::eParse()
{
	CError c_error;

	CCommandParam *pc_command_param;

	for (uint32_t i = 0; i < (uint32_t)v_params.size(); i++)
	{
		pc_command_param = v_params.at(i);

		if (pm_arguments->count(pc_command_param->sGetName()) > 0)
		{
			CError c_set_error = pc_command_param->eSetValue(pm_arguments->at(pc_command_param->sGetName()));

			if (c_set_error)
			{
				c_error = c_set_error;
			}//if (c_set_error)
		}//if (pm_arguments->count(pc_command_param->sGetName()) > 0)
		else 
		{
			if (!pc_command_param->bIsObligatory())
			{
				if (pc_command_param->bHasDefaultValue())
				{
					pc_command_param->vSetDefaultValue();
				}//if (pc_command_param->bHasDefaultValue())
			}//if (!pc_command_param->bIsObligatory())
			else
			{
				c_error.vSetError(CError::iERROR_CODE_SYSTEM_MISSING_ARGUMENT, pc_command_param->sGetName());
			}//else if (!pc_command_param->bIsObligatory())
		}//else if (pm_arguments->count(pc_command_param->sGetName()) > 0)
	}//for (uint32_t i = 0; i < (uint32_t)v_params.size(); i++)

	v_params.clear();

	return c_error;
}//CError CCommandParamParser::eParse()