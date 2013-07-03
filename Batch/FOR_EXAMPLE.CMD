@echo off
REM /****************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                   */
REM /* Created On  : 05 Jan 2000                                    */
REM /* Description : Shows how to generate a simple data file using */
REM /*               the DIR command.                               */ 
REM /****************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm       */
REM /****************************************************************/
FOR /F "tokens=*" %%a in (input.txt) do @echo %%a 
REM FOR /F "tokens=*" %%a in (input.txt) do call test2.bat %%a