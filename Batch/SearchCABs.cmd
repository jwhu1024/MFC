@echo off
REM /*****************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                    */
REM /* Created On  : 29 Mar 2000                                     */
REM /* Description : This batch file can be used to search for a     */
REM /*               specific file in one or more cabinet files. Very*/
REM /*               useful when you want to extract just that one   */
REM /*               file out & don't know which CAB file contains it*/
REM /*               The 1st parameter is the directory that contains*/
REM /*               the CAB files and the 2nd parameter is filename.*/
REM /*               Only a part of the filename can also be passed. */
REM /*****************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm         */
REM /*****************************************************************/
@echo off
setlocal
set cabdir=%1
set searchstr=%2
if (%cabdir%) == () set cabdir=.
if (%searchstr%) == () goto usage
for %%C in (%cabdir%\*.cab) do (
	echo Processing %%C...&extract /D %%C|findstr /I "%searchstr%"&echo ==============================
)
endlocal
goto end

:usage
echo Usage: %0 ^<directory name^> ^<search string^>
echo ^<directory name^> specifies the location of the CAB files
echo ^<search string^> specifies the filename to look for

:end
