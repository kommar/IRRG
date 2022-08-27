#include "StringUtils.h"

CString StringUtils::sGetDirFromPath(CString  sPath)
{
	CString s_result;

	int  i_index;
	i_index = sPath.ReverseFind('\\');
	i_index++;//we include '\'

	if ((0 < i_index) && (i_index < sPath.GetLength()))
		s_result = sPath.Mid(0, i_index);
	else
		s_result = sPath;

	return(s_result);
}//CString StringUtils::sGetDirFromPath(CString  sPath)

void StringUtils::vAppendEndLine(CString *psString)
{
	psString->AppendChar('\n');
}//void StringUtils::vAppendEndLine(CString *psString)