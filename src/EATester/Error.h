#ifndef ERROR_H
#define ERROR_H

#define ERROR_NO_ERROR_CODE 0

#define ERROR_LOG_FILE "error_%.4d.log"
#define ERROR_LOG_FILE_MAX_SIZE 500000

#include <atlstr.h>
#include <cstdint>
#include <vector>

using namespace std;

class CError
{
public:
	CError(int  iErrorParent = iERROR_PARENT_UNKNOWN);

	static uint32_t iADD_ERROR(CString sName);
	static uint32_t iADD_ERROR_PARENT(CString sParentName);

	void vSetNoError() { vSetError(ERROR_NO_ERROR_CODE); };
	void vSetError(uint32_t iCode) { vSetError(iCode, ""); };
	void vSetError(CString sMessage) { vSetError(iERROR_CODE_UNKNOWN, sMessage); };
	void vSetError(uint32_t iCode, CString sMessage);

	uint32_t iGetParent() { return i_err_parent; };
	uint32_t iGetCode() { return i_code; };
	CString sGetMessage() { return s_message; };

	operator bool() { return i_code != ERROR_NO_ERROR_CODE; }

	static uint32_t iERROR_PARENT_CERROR;
	static uint32_t iERROR_PARENT_UNKNOWN;
	static uint32_t iERROR_PARENT_LOAD_ZKEY_FILE;
	

	static uint32_t iERROR_CODE_SYSTEM_MISSING_ARGUMENT;
	static uint32_t iERROR_CODE_SYSTEM_CANNOT_PARSE_ARGUMENT;
	static uint32_t iERROR_CODE_SYSTEM_OUT_OF_BOUND_ARGUMENT;
	static uint32_t iERROR_CODE_SYSTEM_ARGUMENT_WRONG_VALUE;
	static uint32_t iERROR_CODE_SYSTEM_MODE_NOT_FOUND;
	static uint32_t iERROR_CODE_SYSTEM_FILE_NOT_FOUND;
	static uint32_t iERROR_CODE_SYSTEM_WRONG_FILE_PATH;
	static uint32_t iERROR_CODE_SYSTEM_SETTINGS_CANNOT_READ_LINE;
	static uint32_t iERROR_CODE_SYSTEM_MISMATCHING_ARGUMENT;
	static uint32_t iERROR_CODE_SYSTEM_ARGUMENT_WRONG_NUMBER_OF_VALUES;
	static uint32_t iERROR_CODE_SYSTEM_ARGUMENT_WRONG_NUMBER_OF_EVALUATION_ELEMENTS;

	static uint32_t iERROR_CODE_OPERATOR_NOT_FOUND;
	static uint32_t iERROR_CODE_OPERATOR_MISSING_ARGUMENT;

	static uint32_t iERROR_CODE_UNKNOWN;

	

private:
	void v_generate_log();

	static vector<CString> vERROR_NAMES;
	static vector<CString> vERROR_PARENTS;
	
	uint32_t i_code;
	uint32_t i_err_parent;
	CString s_message;

	uint16_t i_file_index;
};//class CError

#endif//ERROR_H