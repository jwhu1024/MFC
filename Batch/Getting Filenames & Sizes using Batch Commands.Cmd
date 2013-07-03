@echo off
REM /********************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                       */
REM /* Created On  : 05 Jan 2000                                        */
REM /* Description : Shows how to generate a simple data file using the */
REM /*               DIR command. Will work for short filenames	only.   */
REM /********************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm           */
REM /********************************************************************/
for /F "skip=5 tokens=3,4" %%i in ('dir/a-d') do @echo %%j %%i|findstr /V ", free"