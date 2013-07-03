@echo off
REM /*****************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                    */
REM /* Created On  : 24 Aug 1999                                     */
REM /* Description : This one demonstrates how to perform arithmetic */
REM /*               operations. This one converts HEX values of the */
REM /*               IP address to their decimal equivalents.        */
REM /*****************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm         */
REM /*****************************************************************/
setlocal
set /A ip1=0x%1
set /A ip2=0x%2
set /A ip3=0x%3
set /A ip4=0x%4
Echo IP Address is: %ip1%.%ip2%.%ip3%.%ip4%
endlocal
