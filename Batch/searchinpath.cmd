@echo off
REM /******************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                     */
REM /* Created On  : 02 Sep 1998                                      */
REM /* Description : This CMD script can be used to search for a file */
REM /*               in the system PATH variable. This is useful when */
REM /*               you want to determine say from which path a DLL  */
REM /*               will be loaded assuming the current directory of */
REM /*               the executable doesn't have the DLL.             */
REM /******************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm         */
REM /******************************************************************/
setlocal
set foundpath=%~dp$path:1
set foundfullpath=%~$path:1
if ("%foundpath%")==("") goto notfound
echo File %1 was found in %foundpath%
echo The full path of the file name is %foundfullpath%
goto end

:notfound
echo File %1 was not found in the environment path.
goto end

:end
endlocal
