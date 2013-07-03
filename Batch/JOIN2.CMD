@echo off
REM /*****************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                    */
REM /* Created On  : 03 Apr 1998                                     */
REM /* Description : A simple batch file to join individual script   */
REM /*               files in a directory to a single file. This CMD */
REM /*               script also adds GO after each script file and  */
REM /*               PRINT messages based on the filename. This can  */
REM /*               be mainly used on SPs' scripts. This also gives */
REM /*               EXECUTE permissions on the SPs. The assumption  */
REM /*               is that the filename is the same as the SP.     */
REM /*****************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm         */
REM /*****************************************************************/
if (%1) == () goto default
set script=%1
goto start
:default
set script=scripts
:start
del %script%.sql 2>NUL
echo SET NOCOUNT ON>>%script%.txt
echo GO>>%script%.txt
for %%i in (*.sql *.*.prc) do @echo.>>%script%.txt&echo Print 'Creating %%~ni...'>>%script%.txt&echo.>>%script%.txt&copy /A /V %script%.Txt + %%i %script%.txt 1>NUL&echo GRANT EXECUTE ON %%~ni TO public>>%script%.txt&echo GO>>%script%.txt&echo.>>%script%.txt
if errorlevel 1 echo Copy was unsuccessful.
ren %script%.txt %script%.sql
