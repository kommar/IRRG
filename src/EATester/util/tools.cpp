//#include "stdafx.h"

#include  "tools.h"


//#define  s_DEBUG_PATH  "G:\\zzz_badania_test\\"
//#define  s_DEBUG_PATH  "D:\\michal\\zz_buf\\"

#define  s_DEBUG_PATH  "C:\\Projekty\\projekty_pwr\\island_ga\\x64\\Release\\"




CString  Tools::sReadLine(FILE  *pfSource,  CString  *psComment /*= NULL*/)
{
	char  c_buf, c_comment_buf;
	CString  s_res,  s_comment;

	bool  b_comment  =  false;


	//init
	c_buf  =  'a';
	c_comment_buf  =  'a';
	s_comment  =  "";
	s_res  =  "";
	while( (!feof(pfSource))&&(c_buf != '\n') )
	{
		fscanf(pfSource, "%c",&c_buf);

		if  (c_buf != '\n')
		{

			if  (b_comment  ==  false)
			{
			
				if  (c_buf  ==  '\\') //everything that after comment sign '//' is ignored
				{
					if  (c_comment_buf  ==  '\\')
						b_comment  =  true;
					else
						c_comment_buf  =  '\\';
					
				}//if  (c_buf  ==  '\\')
				else
				{
					if  (c_comment_buf  ==  '\\')  s_res  =  s_res  +  c_comment_buf; //if something was bufferred we have to give it back
					s_res  =  s_res  +  c_buf;
					c_comment_buf  =  'a';//we have to reset the comment buffer
				}//else  if  (c_buf  ==  '\\')

			}//if  (b_comment  ==  false)
			else
			{
				s_comment  =  s_comment  +  c_buf;			
			}//else  if  (b_comment  ==  false)
		}//if  (c_buf != '\n')

	}//while( (!eof(pfSource))&&(c_buf != '\n') )

	if  (psComment  !=  NULL)  *psComment  =  s_comment;
	return(s_res);
}//CString  Tools::sReadLine(FILE  *pfSource)



int  Tools::iReadLine(FILE  *pfSource,   CString  *psComment)
{
	CString  s_result;
	int  i_result;

	s_result  =  Tools::sReadLine(pfSource,  psComment);
	i_result  =  atoi(s_result);

	return(i_result);
};//int  Tools::iReadLine(FILE  *pfSource,   CString  *psComment  =  NULL)



double  Tools::dReadLine(FILE  *pfSource,   CString  *psComment)
{
	CString  s_result;
	double  d_result;

	s_result  =  Tools::sReadLine(pfSource,  psComment);
	d_result  =  atof(s_result);

	return(d_result);
};//double  Tools::dReadLine(FILE  *pfSource,   CString  *psComment)



CString Tools::sExtractFromString(CString  sLineToSearch, int  iIndex /*= 0*/, int  *piFinishIndex )
{
	CString  s_result;

	char  c_buf;
	bool  b_analyzing = true;
	bool  b_string_started = false;

	int  ii;
	for (ii = iIndex; (ii < sLineToSearch.GetLength()) && (b_analyzing == true); ii++)
	{
		b_analyzing = false;
		c_buf = sLineToSearch.GetAt(ii);

		if ((c_buf == ' ') || (c_buf == '\t'))
		{
			if (b_string_started == false)  b_analyzing = true;
		}//if ((c_buf == ' ') || (c_buf == '\t'))
		else
		{
			s_result += c_buf;
			b_string_started = true;
			b_analyzing = true;
		}//else if ((c_buf == ' ') || (c_buf == '\t'))

	}//for  (int  ii = 0;  ii  <  sLineToSearch.GetLength();  ii++)


	if (piFinishIndex != NULL)  *piFinishIndex = ii;
	return(s_result);
}//CString Tools::sExtractFromString(CString  sLineToSearch, int  iIndex /*= 0*/, int  *piFinishIndex )



int  Tools::iExtractFromString(CString  sLineToSearch, int  iIndex /*= 0*/, int  *piFinishIndex )
{
	CString  s_result;
	int  i_result;

	char  c_buf;
	bool  b_analyzing  =  true;
	bool  b_number_started = false;

	int  ii;
	for  (ii = iIndex;  (ii  <  sLineToSearch.GetLength())&&(b_analyzing  ==  true);  ii++)
	{
		b_analyzing  =  false;
		c_buf  =  sLineToSearch.GetAt(ii);

		if  ( 
			((c_buf  >=  '0')&&(c_buf  <=  '9'))
			||
			(c_buf  ==  '-') 
			)
		{
			s_result  +=  c_buf;
			b_analyzing  =  true;
			b_number_started = true;
		}//if  ( (c_buf  >=  '0')&&(c_buf  <=  '9') )

		if  ( (c_buf  == ' ')||(c_buf  == '\t') )
		{
			if  (b_number_started == false)  b_analyzing  =  true;
		}//if  (c_buf  == ' ')  
		
	}//for  (int  ii = 0;  ii  <  sLineToSearch.GetLength();  ii++)

	
	if  (piFinishIndex != NULL)  *piFinishIndex = ii;
	i_result =  atoi(s_result);	
	return(i_result);
}//int  Tools::iExtractFromString(sLineToSearch, int  iIndex /*= 0*/)



double  Tools::dExtractFromString(CString  sLineToSearch, int  iIndex, int  *piFinishIndex )
{
	CString  s_result;
	double  d_result;

	char  c_buf;
	bool  b_analyzing  =  true;
	bool  b_number_started = false;

	int  ii;
	for  (ii = iIndex;  (ii  <  sLineToSearch.GetLength())&&(b_analyzing  ==  true);  ii++)
	{
		b_analyzing  =  false;
		c_buf  =  sLineToSearch.GetAt(ii);

		if  ( ((c_buf  >=  '0')&&(c_buf  <=  '9'))||(c_buf  ==  '.') )
		{
			s_result  +=  c_buf;
			b_analyzing  =  true;
			b_number_started = true;
		}//if  ( (c_buf  >=  '0')&&(c_buf  <=  '9') )

		if  ( (c_buf  == ' ')||(c_buf  == '\t') )
		{
			if  (b_number_started == false)  b_analyzing  =  true;
		}//if  (c_buf  == ' ')  
		
	}//for  (int  ii = 0;  ii  <  sLineToSearch.GetLength();  ii++)

	if  (piFinishIndex != NULL)  *piFinishIndex = ii;
	d_result =  atof(s_result);	

	return(d_result);
}//double  Tools::dExtractFromString(CString  sLineToSearch, int  iIndex /*= 0*/)






void  Tools::vReportInFile(CString  sFile, CString  sValue, bool  bMakeNew)
{
    FILE  *pf_save;
    CString  s_buf;
    s_buf.Format("%s%s", s_DEBUG_PATH, sFile);

	if  (bMakeNew  ==  true)
		pf_save = fopen(s_buf, "w+");
	else
		pf_save = fopen(s_buf, "a+");

    if  (pf_save  !=  NULL)
    {
            fprintf(pf_save, "%s\n", sValue);
            fclose(pf_save);
    }//if  (pf_save  !=  NULL)
}//void  vReportInFile(string  sFile, double  dValue)


void  Tools::vShowMessage(CString sMessage)
{
    vShowMessage("", sMessage);
}//void  vShowMessage(CString sMessage)


void  Tools::vShowMessage(CString sTitle, CString sMessage)
{
	::MessageBox(NULL, (LPCSTR)sMessage, (LPCSTR)sTitle, MB_OK);
    //QMessageBox  c_mess;
    //c_mess.setText(sMessage);
    //c_mess.exec();
};//void  vShowMessage(CString sTitle, CString sMessage)



void  Tools::vShow(double  dValue)
{
    CString  s_buf;
    s_buf.Format("%.2lf", dValue);
    vShowMessage(s_buf);
}//void  vShow(double  dValue)


void  Tools::vShow(CString  sMessage)
{
    vShowMessage(sMessage);
}//void  vShow(double  dValue)



CString  Tools::sGenerateGenotypeWithHighlightedGene(int  *piGenotype, int iLen, int iHighlightedGene)
{
	CString  s_line, s_buf;

	int  i_high_gene = 0;
	s_line = "";
	for  (int  ii = 0; ii < iHighlightedGene; ii++)
	{
		if  (ii == ii / 3 * 3) s_line += " ";

		if  (piGenotype[ii]  <  0)
			s_buf = "?";
		else
			s_buf.Format("%d", piGenotype[ii]);
		s_line += s_buf;
		i_high_gene = ii;
	}//for  (int  ii = 0; ii < iHighlightedGene; ii++)
	i_high_gene++;


	
	int  i_start;
	if  (iHighlightedGene  <  0)
		i_start = 0;
	else
	{

		s_buf.Format("  %d  ", piGenotype[i_high_gene]);
		s_line += s_buf;
		i_start = iHighlightedGene + 1;
	}//else

	for  (int  ii = i_start; ii < iLen; ii++)
	{
		if  (ii == ii / 3 * 3) s_line += " ";

		if  (piGenotype[ii]  <  0)
			s_buf = "?";
		else
			s_buf.Format("%d", piGenotype[ii]);
		s_line += s_buf;
	}//for  (int  ii = 0; ii < iHighlightedGene; ii++)
					
	return(s_line);
}//CString  Tools::sGenerateGenotypeWithHighlightedGene(int  *piGenotype, int iLen, int iHighlightedGene)




