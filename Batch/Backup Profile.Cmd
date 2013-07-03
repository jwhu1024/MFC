@echo off
REM /******************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                     */
REM /* Created On  : 21 Nov 1999                                      */
REM /* Description : This CMD script is used to backup the NT profile */
REM /*		     on my Windows 2000 Server at home. This script   */
REM /*		     demonstrates how to generate file names with date*/
REM /*		     & time as part of the name using the FOR extn.   */
REM /******************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm         */
REM /******************************************************************/
for /F "eol=; tokens=1-4 delims=/ " %%i in ('date/T') do @c:\progra~1\winzip\wzzip -ex -p -r -x@"%userprofile%\My Documents\CMD Scripts\exclude.txt" -a "C:\Temp\My Profile Backup (%%i-%%l%%k%%j)" "%userprofile%"
