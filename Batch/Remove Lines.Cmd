@echo off
REM /*****************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                    */
REM /* Created On  : 24 Feb 2000                                     */
REM /* Description : This batch file can be used to remove specified */
REM /*		     number of lines from a data file. The file ext  */
REM /*		     is assumed as .dat & it works off the current   */
REM /*		     directory.	Removes the 1st line alone if value  */
REM /*		     is not passed.				     */
REM /*****************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm         */
REM /*****************************************************************/
setlocal
set numlines=%1
if (%numlines%)==() set numlines=1
for %%i in (*.dat) do @ren %%i %%~ni.bak&for /F "skip=%numlines% delims=~" %%l in ('type %%~ni.bak') do @echo %%l>>%%i
endlocal
