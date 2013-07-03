@echo off
REM /*****************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                    */
REM /* Created On  : 03 Feb 2000                                     */
REM /* Description : This batch file can be used to list the status  */
REM /*               of the printers, jobs on a computer.            */
REM /*		     Escaping of the pipe symbol using ^ character   */
REM /*		     was suggested by Klaus Seeling, NT Mag Forum Pro*/
REM /*****************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm         */
REM /*****************************************************************/
if (%1)==() goto error
setlocal
set servername=%1
echo Status of printers on \\%servername%:
echo Name                         Job #      Size            Status
echo -------------------------------------------------------------------------------
for /F "tokens=1" %%i in ('net view \\%servername%^|findstr /I "print"') do net print \\%servername%\%%i|findstr /I "%%i"
echo -------------------------------------------------------------------------------
endlocal
goto end

:error
echo Please specify a computer name.
goto end

:end
