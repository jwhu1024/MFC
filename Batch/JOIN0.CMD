@echo off
REM /*****************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                    */
REM /* Created On  : 21 Oct 1997                                     */
REM /* Description : A simple batch file to join individual script   */
REM /*               files in a directory to a single file. This CMD */
REM /*               script also adds GO after each script file and  */
REM /*               PRINT messages based on the filename.           */
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
for %%i in (*.sql *.*.prc *.*.trg *.*.tab *.*.viw) do @echo.>>%script%.txt&echo Print 'Creating %%~ni...'>>%script%.txt&echo GO>>%script%.txt&echo.>>%script%.txt&copy /A /V %script%.Txt + %%i %script%.txt 1>NUL&echo.>>%script%.txt
if errorlevel 1 echo Copy was unsuccessful.
ren %script%.txt %script%.sql
