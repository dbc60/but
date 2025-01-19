@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

SET DIR_CMD=%~dp0
SET DIR_CMD=%DIR_CMD:~0,-1%
SET DIR_LOCAL=%DIR_CMD%\local
CALL %DIR_CMD%\options.cmd %*
CALL %DIR_CMD%\setup.cmd %*

:: The build defaults to debug unless release is explicitly passed in
IF NOT "%release%"=="" (
    if %release% EQU 1 (
        SET REL_OPT=release
    )
)

IF NOT EXIST "%DIR_OUT_LIB%\exceptions" (
    CALL %DIR_LOCAL%\exceptions.cmd build %REL_OPT%
    if errorlevel 1 (
        echo failed to build exceptions for BUT
        GOTO :EOF
    )
)

if %verbose% EQU 1 (
    ECHO Build Basic Unit Test %*
)
call %DIR_CMD%\local\but.cmd %*
if errorlevel 1 (
    echo failed to build the Basic Unit Test library and driver
    GOTO :EOF
)

if %test% EQU 1 (
    if %verbose% EQU 1 (
        ECHO Run BUT unit tests
        ECHO.
    )
    pushd %DIR_OUT_BIN%
    but\but.exe ^
        but\butts.dll
    popd
)
ENDLOCAL
