@echo off
REM /******************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                     */
REM /* Created On  : 23 June 2000                                     */
REM /* Description : This batch file shows how to check the date of a */
REM /*               file. This script can be use to verify if a file */
REM /*               was created today programmatically.              */
REM /******************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm          */
REM /******************************************************************/
REM
setlocal
for /F "tokens=6" %%d in ('net time \\%COMPUTERNAME%^|findstr /I /C:"%COMPUTERNAME%"') do set today=%%d
for /F %%d in ('dir/n %1^|findstr /I /C:"%~n1"') do set filedate=%%d
REM
REM This IF takes care of date output like d/mm/yy or d/m/yy
REM by padding 'd' with zero. ( Needed for NT40 compatability )
REM
if (%today:~1,1%)==(/) set today=0%today%
REM
REM This IF takes care of date output like dd/mm/yy or dd/m/yy
REM by padding 'm' with zero. ( Needed for NT40 compatability )
REM
if (%today:~4,1%)==(/) set today=%today:~0,3%0%today:~3,6%
REM
if (%today:~0,6%%today:~8,2%)==(%filedate%) (
echo This file was created today.
) ELSE (
echo This file was not created today.
)
endlocal
