@echo off
REM /*****************************************************************/
REM /* Created By  : Umachandar Jayachandran (UC)                    */
REM /* Created On  : 24 Feb 2000                                     */
REM /* Description : This batch file can be used to replace certain  */
REM /*		     strings globally by searching through files in  */
REM /*		     the specified directories. In this example, only*/
REM /*		     directories with "scripts" or "Vfp" in the name */
REM /*		     is filtered out. This script uses "REPLACE.SCR" */
REM /*		     file to specify the strings to be searched for &*/
REM /*		     replaced by "MUNGE.EXE".                        */
REM /*****************************************************************/
REM /* Resources At: http://www.umachandar.com/resources.htm         */
REM /*****************************************************************/
cd %userprofile%\my documents
for /F "delims=~" %%D in ('dir /ad /a /b^|findstr /I "scripts vfp"') do @for /F "delims=~" %%F in ('dir /s /b "%%D"') do @munge c:\replace.scr -i -l -t "%%F" 2>Replaced.txt
