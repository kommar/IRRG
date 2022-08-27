#include "IOUtils.h"

#include <fstream>

using namespace std;

bool IOUtils::bFileExists(CString sFilePath)
{
	ifstream f_file(sFilePath);
	bool b_exists = f_file.good();
	f_file.close();

	return b_exists;
}//bool IOUtils::bFileExists(CString sFilePath)