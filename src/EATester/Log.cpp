#include "Log.h"

#include <fstream>
#include <iostream>



CLog::~CLog()
{
	vClear();
}//CLog::~CLog()

void CLog::vPrintLine(CString sMessage, bool bEcho, int iMessOffset)
{
	if (bEcho)
	{
		if  ( (i_log_system_to_show < 0)|| (i_log_system_to_show == iMessOffset))  cout << sMessage << endl;
	}//if (bEcho)

	while (v_messages.size() <= iMessOffset)  v_messages.push_back(vector<CString> ());
	v_messages.at(iMessOffset).push_back(sMessage);
}//void CLog::vPrintLine(CString sMessage, bool bEcho)


CError CLog::e_save_single_mess_system(ostream *psOutput, int  iLogOffset)
{
	CError c_error(iERROR_PARENT_CLOG);

	if (psOutput->good())
	{
		if (iLogOffset < v_messages.size())
		{
			for (uint32_t i = 0; i < (uint32_t)v_messages.at(iLogOffset).size(); i++)
			{
				*(psOutput) << v_messages.at(iLogOffset).at(i) << endl;
			}//for (uint32_t i = 0; i < (uint32_t)v_messages.size(); i++)
		}//if (iLogOffset < v_messages.size())
	}//if (psOutput->good())
	else
	{
		c_error.vSetError(iERROR_CODE_LOG_CANNOT_WRITE_TO_STREAM);
	}//else if (psOutput->good())

	return c_error;
}//CError CLog::eSave(ostream *psOutput)

CError CLog::eSave(ostream *psOutput, int  iLogOffset)
{
	CError c_error(iERROR_PARENT_CLOG);

	if  (iLogOffset >= 0)
		c_error = e_save_single_mess_system(psOutput, iLogOffset);
	else
	{
		for (int ii = 0; ii < v_messages.size(); ii++)
		{
			*(psOutput) << endl << endl << "LOG SYSTEM: " << ii << endl;
			e_save_single_mess_system(psOutput, ii);
		}//for (int ii = 0; ii < v_messages.size(); ii++)
			
	}//else  if  (iLogOffset >= 0)

	return c_error;
}//CError CLog::eSave(ostream *psOutput)


CError CLog::eSave(CString sFilePath, int  iLogOffset)
{
	CError c_error(iERROR_PARENT_CLOG);

	s_log_file_path = sFilePath;
	ofstream f_log(sFilePath);

	if (f_log.good())
	{
		c_error = eSave(&f_log, iLogOffset);
	}//if (f_log.good())
	else
	{
		c_error.vSetError(CError::iERROR_CODE_SYSTEM_WRONG_FILE_PATH, sFilePath);
	}//else if (f_log.good())

	return c_error;
}//CError CLog::eSave(CString sFilePath)



uint32_t CLog::iERROR_PARENT_CLOG = CError::iADD_ERROR_PARENT("CLog");
uint32_t CLog::iERROR_CODE_LOG_CANNOT_WRITE_TO_STREAM = CError::iADD_ERROR("iERROR_CODE_LOG_CANNOT_WRITE_TO_STREAM");