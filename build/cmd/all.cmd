@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

SET DIR_CMD=%~dp0
SET DIR_CMD=%DIR_CMD:~0,-1%
SET DIR_LOCAL=%DIR_CMD%\local
CALL %DIR_CMD%\options.cmd %*
CALL %DIR_CMD%\setup.cmd %*

if %verbose% EQU 1 (
    ECHO "Build the Exceptions library (%*)"
)
call %DIR_LOCAL%\exceptions.cmd %*
if errorlevel 1 (
    ECHO "failed to build the Exceptions library (%*)"
    GOTO :EOF
)

if %verbose% EQU 1 (
    ECHO "Build the Basic Unit Test library and driver (%*)"
)
call %DIR_LOCAL%\but.cmd %*
if errorlevel 1 (
    ECHO "failed to build the Basic Unit Test library and driver (%*)"
    GOTO :EOF
)


if %test% EQU 1 (
    if %verbose% EQU 1 (
        ECHO "Run all unit tests"
        ECHO.
    )
    TITLE Unit Tests
    pushd %DIR_OUT_BIN%
    but\but.exe ^
        exceptions\butts.dll ^
        but\butts.dll
    popd
)
ENDLOCAL
