@echo off
REM /*****************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                    */
REM /* Created On  : 22 Jan 2000                                     */
REM /* Description : I wrote this batch file to replace the HTML tag */
REM /*               related characters like '<', '>', '"' with the  */
REM /*               corresponding codes. The file is also copied to */
REM /*               a database table to be used for web page gentn. */
REM /*****************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm         */
REM /*****************************************************************/
setlocal
set filename=%1
CD %userprofile%\my documents
COPY /Y %filename% C:\Temp.Txt
MUNGE c:\munge.scr -i -l -t c:\temp.txt
TEXTCOPY /S. /U  /P  /D "[My Personal DB]" /T "[My Resource Files]" /C Contents /W "WHERE [RelativePath] + '\' + [Filename] = '.%filename:~2,-1%'" /F "C:\Temp.Txt" /I
endlocal
