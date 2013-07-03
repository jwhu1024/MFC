@echo off
REM /******************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                     */
REM /* Created On  : 23 June 2000                                     */
REM /* Description : This batch file can be used to get the specified */
REM /*               line from any text file. The lines must be ended */
REM /*               by CR + LF. The FOR statement can be modified to */
REM /*               accomodate other EOL characters.                 */
REM /******************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm          */
REM /******************************************************************/
setlocal
set /A lines=%2 - 1
if %lines% LEQ 0 set lines=0
if %lines% EQU 0 set skiplines=
if %lines% NEQ 0 set skiplines=skip=%lines%
for /F "%skiplines% tokens=* " %%i in (%1) do @echo %%i&goto end
:end
endlocal
