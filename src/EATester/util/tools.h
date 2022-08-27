#pragma once

//tools
#include  "atlstr.h"  //CString
#include  "atlpath.h"

//system tools
#include  "..\Error.h"





namespace Tools
{
	CString  sReadLine(FILE  *pfSource,   CString  *psComment  =  NULL);
	int  iReadLine(FILE  *pfSource,   CString  *psComment  =  NULL);
	double  dReadLine(FILE  *pfSource,   CString  *psComment  =  NULL);
	CString  sExtractFromString(CString  sLineToSearch, int  iIndex /*= 0*/, int  *piFinishIndex = NULL);
	int  iExtractFromString(CString  sLineToSearch, int  iIndex /*= 0*/, int  *piFinishIndex = NULL );
	double  dExtractFromString(CString  sLineToSearch, int  iIndex, int  *piFinishIndex = NULL );

	CString  sGenerateGenotypeWithHighlightedGene(int  *piGenotype, int iLen, int iHighlightedGene);

	void  vShowMessage(CString sMessage);
	void  vShowMessage(CString sTitle, CString sMessage);
	void  vShow(double  dValue);
	void  vShow(CString  sMessage);
	void  vReportInFile(CString  sFile, CString  sValue, bool  bMakeNew = false);
};//namespace Tools