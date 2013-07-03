@echo off
REM /******************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                     */
REM /* Created On  : 22 Sep 1998                                      */
REM /* Description : A simple batch file to demonstrate registry key  */
REM /*               manipulation. This one changes the 3DTEXT screen */
REM /*               message text - i used to schedule this using AT  */
REM /*               to get a rudimentary screen saver that changes   */
REM /*               messages during the day.                         */
REM /******************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm          */
REM /******************************************************************/
setlocal
set regfile=c:\temp\3dtext-mesg-change.reg
set mesg_type=%1
if (%mesg_type%)==() set mesg_type=default
goto %mesg_type%

:lunch_mesg
set message=Out to Lunch...
goto set_mesg

:not_in_office_mesg
set message=Not in Office...
goto set_mesg

:default
set message=Ramble On...

:set_mesg
echo REGEDIT4>%regfile%
echo.>>%regfile%
echo [HKEY_CURRENT_USER\Control Panel\Screen Saver.3DText]>>%regfile%
echo "Text"="%message%">>%regfile%
echo.>>%regfile%

regedit /s %regfile%
endlocal
