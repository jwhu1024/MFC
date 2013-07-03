@echo off
REM /******************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                     */
REM /* Created On  : 06 Dec 1999                                      */
REM /* Description : Batch file to delete temporary files. This can be*/
REM /*		     run from any directory in the NT CMD prompt. The */
REM /*		     current directory will be restored after the CMD */
REM /*		     script execution is completed.		      */
REM /******************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm          */
REM /******************************************************************/
setlocal
for /F "eol= delims=~" %%d in ('CD') do set curdir=%%d
pushd %curdir%
cd c:\temp
del *.tmp /F /Q
cd %USERPROFILE%\local settings\temp
del *.tmp /F /Q
popd %curdir%
endlocal
