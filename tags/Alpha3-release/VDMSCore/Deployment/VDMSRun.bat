@echo off

REM Launch VDMS
dosdrv.exe /i:VDMS.INI

REM Check if VDMS loaded successfully
if errorlevel 3 goto error3
if errorlevel 2 goto error2
if errorlevel 1 goto error1

REM Free up some environment space
set VDMSPath=

REM Set the BLASTER variable
REM [TO THE USER]: make sure it's consistent with VDMS.INI !
set BLASTER=A220 I7 D1 H5 P330

REM Run the game
echo.
call %1
echo.

REM Close the window
pause
exit

REM Command-line errors
:error1
echo.
echo VDMS encountered an error while parsing the command line.
echo If you modified VDMSRun.bat, please review your modifications and possibly
echo restore the old version of VDMSRun.bat.
goto end

REM VDD load errors
:error2
echo.
echo VDMS encountered an error while loading a DLL.  Please make sure that VDMS
echo is properly installed, and that the file VDDLoader.dll is in the PATH or
echo in the current directory.  It is also possible that you have older
echo versions of one or more system DLLs, in which case you will have to
echo download newer versions from the VDMS site.
goto end

REM VDMConfig errors
:error3
echo.
echo VDMS loaded properly, but encountered an error while initializing.
goto end

:end
echo.
echo Don't hesitate to seek support on the VDMS site, http://ntvdm.cjb.net/.
pause
exit
01234567890123456789012345678901234567890123456789012345678901234567890123456789