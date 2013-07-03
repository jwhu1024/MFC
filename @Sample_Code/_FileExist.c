1.

BOOL FIleIsExist(Char* pszFile)
{
     CFileStatus rStatus;
     return CFile::GetStatus(pszFile, rStatus);
}

2.

BOOL FIleIsExist(Char* pszFile)
{
int iRet;
#ifdef _UNICODE
iRet = _waccess(pszFile, 04);
#else
iRet = _access(pszFile, 04);
#endif
return (iRet == 0);
}
