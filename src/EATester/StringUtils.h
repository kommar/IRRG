#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <atlstr.h>
#include <xhash>

namespace StringUtils
{
	CString sGetDirFromPath(CString  sPath);

	void vAppendEndLine(CString *psString);
}//namespace StringUtils

namespace std
{
	template <>
	struct hash<CString>
	{
		size_t operator()(const CString &sText) const { return stdext::hash_value((LPCTSTR)sText); };
	};//struct hash<CString>
}//namespace std

#endif//STRING_UTILS_H