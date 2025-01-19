@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

SET DIR_CMD=%~dp0
SET DIR_CMD=%DIR_CMD:~0,-1%
CALL %DIR_CMD%\options.cmd %*
CALL %DIR_CMD%\setup.cmd %*

call %DIR_CMD%\local\exceptions.cmd %*
if errorlevel 1 (
    echo failed to build the Exceptions library and test suite
    GOTO :EOF
)

IF NOT EXIST "%DIR_OUT_BIN%\but\but.exe" (
    if %verbose% EQU 1 (
        ECHO Build BUT driver build
    )
    call %DIR_CMD%\but.cmd build %REL_OPT%
    if errorlevel 1 (
        echo failed to build the Basic Unit Test library and driver
        GOTO :EOF
    )
)

if %test% EQU 1 (
    if %verbose% EQU 1 (
        ECHO Run Exceptions unit tests.
        ECHO.
    )
    pushd %DIR_OUT_BIN%
    but\but.exe ^
        exceptions\butts.dll
    popd
)
ENDLOCAL
