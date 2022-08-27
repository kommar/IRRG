#ifndef SYSTEM_H
#define SYSTEM_H

#define SYSTEM_ARGUMENT_DESTINATION_FILE "destination_file"
#define SYSTEM_ARGUMENT_MODE "mode"
#define SYSTEM_ARGUMENT_MODE_OPTIMIZATION "optimization"

#define SYSTEM_ARGUMENT_RANDOM_SEED "random_seed"
#define SYSTEM_ARGUMENT_LOG_SYSTEM_OFFSET "log_system"

#define SYSTEM_ARGUMENT_OUTPUT_FILE_PATH "output_file_path" 



#include "CommandExecution.h"
#include "Error.h"
#include "Log.h"
#include "StringUtils.h"
#include "util\tools.h"

#include <atlstr.h>
#include <cstdint>
#include <fstream>
#include <unordered_map>





class CSystem
{
static uint32_t iERROR_CODE_SYSTEM_IN_FILE_NOT_FOUND;
static uint32_t iERROR_CODE_SYSTEM_SETTNGS_FILE_NOT_FOUND;

public:
	CSystem(int iArgCount, char **ppcArgValues, CError *pcError);
	
	~CSystem();

	CError eRun();

private:
	void v_register_command(CCommandExecution *pcCommand) { m_commands.insert(pair<const CString, CCommandExecution*>(pcCommand->sGetName(), pcCommand)); };
	void v_register_commands();

	CError e_create_log_file(CString  sFileName, CString  *psFinalFilename);
	CError e_read_arguments_from_file(int iArgCount, char **ppcArgValues);
	CError e_load_settings(CString sSettingsFile);

	ofstream *pf_log;

	CString  s_result_summary_file;
	ofstream f_result_summary;

	CString s_log_file;
	CLog c_log;

	uint32_t i_random_seed;

	ifstream f_settings;

	unordered_map<CString, CString> m_arguments;
	unordered_map<CString, CCommandExecution*> m_commands;
};//class CSystem

#endif//SYSTEM_H