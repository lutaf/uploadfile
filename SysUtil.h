#ifndef  _ZJY_SYSUTIL_H_
#define  _ZJY_SYSUTIL_H_
#include <windows.h>
#include <string>
using std::string;

//-----------------------------------------------------------------
inline string ExtractFilePath(const string & name)
{
	string::size_type ed;
	ed=name.find_last_of('\\');
	return name.substr(0,ed+1);
}
//-----------------------------------------------------------------
inline string ExtractFileName(const string  & name)
{
	string::size_type ed;
	ed=name.find_last_of('\\');
	return name.substr(ed+1);
}

DWORD GetSizeOfFile(LPCTSTR FileName);
bool IsDirExist(const char *dir);
bool IsFileExist(const char *dir);


#endif
