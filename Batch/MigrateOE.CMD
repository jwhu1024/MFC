@echo off
REM /******************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                     */
REM /* Created On  : 28 June 2000                                     */
REM /* Description : This batch file demonstrates how to make a dialup*/
REM /*               connection from the command-line , retrieve the  */
REM /*               IP address for the dial-up connection & use it to*/
REM /*               access a share on the server. The IP address is  */
REM /*               is determined using IPCONFIG output & assumption */
REM /*               here is that the IP address allocated by the RAS */
REM /*               server is dynamic. In this example, the script   */
REM /*               copies the Outlook Express files from my PC to   */
REM /*               the account on the laptop.                       */
REM /*               This script also shows how to replace spaces in a*/
REM /*               string with nothing viz. "%laptop: =%" .         */
REM /******************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm          */
REM /******************************************************************/
rasdial "Laptop"
for /F "delims=: tokens=2" %%i in ('ipconfig/all^|findstr /I /C:"Default Gateway"') do @set laptop=%%i&set laptop=%laptop: =%
copy /Y "%userprofile%\Local Settings\Application Data\Identities\{2F8B35F6-6A31-47C9-97D0-B573658A80DB}\Microsoft\Outlook Express" "\\%laptop%\c$%userprofile:C:=%\Local Settings\Application Data\Identities\{2F8B35F6-6A31-47C9-97D0-B573658A80DB}\Microsoft\Outlook Express"
rasdial "Laptop" /DISCONNECT
