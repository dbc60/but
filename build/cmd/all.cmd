@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

if NOT EXIST metrics (
    md metrics
)
ctime.exe -begin metrics\all.ctm

SET DIR_CMD=%~dp0
SET DIR_CMD=%DIR_CMD:~0,-1%
CALL %DIR_CMD%\options.cmd %*
CALL %DIR_CMD%\setup.cmd %*

set "args="
for %%A in (%*) do (
    if /I not "%%~A"=="test" if /I not "%%~A"=="clean" if /I not "%%~A"=="cleanall" (
        set "args=!args! %%~A"
    )
    if /I "%%~A"=="test" (
        set "args=!args! build"
    )
)

call %DIR_CMD%\exception.cmd !args!
if errorlevel 1 (
    ctime.exe -end metrics\all.ctm %errorlevel%
    GOTO :EOF
)

call %DIR_CMD%\but.cmd !args!
if errorlevel 1 (
    ctime.exe -end metrics\all.ctm %errorlevel%
    GOTO :EOF
)

ctime.exe -end metrics\all.ctm %errorlevel%

if %test% EQU 1 (
    if %verbose% EQU 1 (
        ECHO "Run all unit tests"
        ECHO.
    )
    TITLE Unit Tests
    pushd %DIR_OUT_BIN%
    but.exe ^
        exception_butts.dll ^
        but_butts.dll
    popd
)
ENDLOCAL
