#include "Error.h"

#include "IOUtils.h"

#include <fstream>
#include <iomanip>
#include <Windows.h>

CError::CError(int  iErrorParent)
{
	i_file_index = 0;
	i_err_parent = iErrorParent;
	
	vSetNoError();
}//CError::CError()


uint32_t CError::iADD_ERROR(CString sName)
{
	vERROR_NAMES.push_back(sName);

	return (uint32_t)vERROR_NAMES.size();
}//uint32_t CError::iADD_ERROR(CString sName)


uint32_t CError::iADD_ERROR_PARENT(CString sParentName)
{
	vERROR_PARENTS.push_back(sParentName);

	return (uint32_t)vERROR_PARENTS.size();
}//static  int  iADD_ERROR_PARENT(CString sParentName)

void CError::vSetError(uint32_t iCode, CString sMessage)
{
	i_code = iCode;
	s_message = sMessage;

	if (*this)
	{
		v_generate_log();
	}//if (*this)
}//void CError::vSetError(uint32_t iCode, CString sMessage)

void CError::v_generate_log()
{
	char pc_application_dir[MAX_PATH];
	
	GetModuleFileNameA(nullptr, pc_application_dir, MAX_PATH);
	PathRemoveFileSpecA(pc_application_dir);
	PathAddBackslashA(pc_application_dir);

	CString s_application_dir(pc_application_dir);
	CString s_log_file_path;

	bool b_log_file_index_found = false;

	fstream f_log_file;
	uint64_t i_log_file_size;

	for (uint16_t i = 0; !b_log_file_index_found; i++)
	{
		s_log_file_path.AppendFormat(CString(ERROR_LOG_FILE), i);

		if (IOUtils::bFileExists(s_log_file_path))
		{
			f_log_file.open(s_log_file_path, fstream::in | fstream::out | fstream::ate);
			i_log_file_size = f_log_file.tellp();

			if (i_log_file_size < ERROR_LOG_FILE_MAX_SIZE)
			{
				b_log_file_index_found = true;
			}//if (i_log_file_size < ERROR_LOG_FILE_MAX_SIZE)
			else
			{
				f_log_file.close();
			}//else if (i_log_file_size < ERROR_LOG_FILE_MAX_SIZE)
		}//if (IOUtils::bFileExists(s_log_file_path))
		else
		{
			f_log_file.open(s_log_file_path, fstream::out);
			b_log_file_index_found = true;
		}//else if (IOUtils::bFileExists(s_log_file_path))
	}//for (uint16_t i = 0; !b_log_file_index_found; i++)

	SYSTEMTIME dt_now;
	GetLocalTime(&dt_now);

	CString s_now;
	s_now.Format("%.4d-%.2d-%.2d %.2d:%.2d:%.2d", dt_now.wYear, dt_now.wMonth, dt_now.wDay, dt_now.wHour, dt_now.wMinute, dt_now.wSecond);

	f_log_file << "Error local time: " << s_now << endl;
	f_log_file << "Error parent: " << vERROR_PARENTS.at(i_err_parent - 1) << endl;
	f_log_file << "Error name:" << endl << vERROR_NAMES.at(i_code - 1) << endl;
	f_log_file << "Error message:" << endl << s_message << endl;
	f_log_file << "______________" << endl << endl;

	f_log_file.close();
}//void CError::v_generate_log()

vector<CString> CError::vERROR_NAMES;
vector<CString> CError::vERROR_PARENTS;


uint32_t CError::iERROR_PARENT_CERROR = iADD_ERROR_PARENT("CError");
uint32_t CError::iERROR_PARENT_UNKNOWN = iADD_ERROR_PARENT("UNKNOWN_ERROR_PARENT");



uint32_t CError::iERROR_CODE_SYSTEM_MISSING_ARGUMENT = iADD_ERROR("SYSTEM_MISSING_ARGUMENT");
uint32_t CError::iERROR_CODE_SYSTEM_CANNOT_PARSE_ARGUMENT = iADD_ERROR("SYSTEM_CANNOT_PARSE_ARGUMENT");
uint32_t CError::iERROR_CODE_SYSTEM_OUT_OF_BOUND_ARGUMENT = iADD_ERROR("SYSTEM_OUT_OF_BOUND_ARGUMENT");
uint32_t CError::iERROR_CODE_SYSTEM_ARGUMENT_WRONG_VALUE = iADD_ERROR("SYSTEM_ARGUMENT_WRONG_VALUE");
uint32_t CError::iERROR_CODE_SYSTEM_MODE_NOT_FOUND = iADD_ERROR("SYSTEM_MODE_NOT_FOUND");
uint32_t CError::iERROR_CODE_SYSTEM_FILE_NOT_FOUND = iADD_ERROR("SYSTEM_FILE_NOT_FOUND");
uint32_t CError::iERROR_CODE_SYSTEM_WRONG_FILE_PATH = iADD_ERROR("SYSTEM_WRONG_FILE_PATH");
uint32_t CError::iERROR_CODE_SYSTEM_SETTINGS_CANNOT_READ_LINE = iADD_ERROR("SYSTEM_SETTINGS_CANNOT_READ_LINE");
uint32_t CError::iERROR_CODE_SYSTEM_MISMATCHING_ARGUMENT = iADD_ERROR("SYSTEM_MISMATCHING_ARGUMENT");
uint32_t CError::iERROR_CODE_SYSTEM_ARGUMENT_WRONG_NUMBER_OF_VALUES = iADD_ERROR("SYSTEM_ARGUMENT_WRONG_NUMBER_OF_VALUES");
uint32_t CError::iERROR_CODE_SYSTEM_ARGUMENT_WRONG_NUMBER_OF_EVALUATION_ELEMENTS = iADD_ERROR("iERROR_CODE_SYSTEM_ARGUMENT_WRONG_NUMBER_OF_EVALUATION_ELEMENTS");

uint32_t CError::iERROR_CODE_OPERATOR_NOT_FOUND = iADD_ERROR("OPERATOR_NOT_FOUND");
uint32_t CError::iERROR_CODE_OPERATOR_MISSING_ARGUMENT = iADD_ERROR("OPERATOR_MISSING_ARGUMENT");

uint32_t CError::iERROR_CODE_UNKNOWN = iADD_ERROR("UNKNOWN");