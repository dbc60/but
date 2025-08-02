@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

if NOT EXIST metrics (
    md metrics
)
ctime.exe -begin metrics\but.ctm

SET PROJECT_NAME="Basic Unit Test"
SET PROJECT_NAME=%PROJECT_NAME:"=%
TITLE %PROJECT_NAME%

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

:: Build the project
IF %build% EQU 1 (
    if %verbose% EQU 1 (
        ECHO.
        ECHO Build the %PROJECT_NAME% driver test suite
    )
    cl %CommonCompilerFlagsFinal% /I"%DIR_INCLUDE%" /DDLL_BUILD ^
        %DIR_REPO%\src\but_butts.c /Fo:%DIR_OUT_OBJ%\ ^
        /Fd:%DIR_OUT_BIN%\but_butts.pdb ^
        /LD /link %CommonLinkerFlagsFinal% /LIBPATH:%DIR_OUT_LIB% ^
        /OUT:%DIR_OUT_BIN%\but_butts.dll /IMPLIB:%DIR_OUT_LIB%\but_butts.lib
    if errorlevel 1 (
        echo failed to build the %PROJECT_NAME% driver test suite
        ctime.exe -end metrics\but.ctm %errorlevel%
        GOTO :EOF
    )

    if %verbose% EQU 1 (
        ECHO.
        ECHO Build the %PROJECT_NAME% driver test-data DLL
    )
    cl %CommonCompilerFlagsFinal% /I"%DIR_INCLUDE%" /DDLL_BUILD ^
    %DIR_REPO%\src\but_test_data.c ^
    /Fo:%DIR_OUT_OBJ%\ /Fd:%DIR_OUT_BIN%\but_test_data.pdb ^
    /LD /link %CommonLinkerFlagsFinal% ^
    /OUT:%DIR_OUT_BIN%\but_test_data.dll ^
    /IMPLIB:%DIR_OUT_LIB%\but_test_data.lib
    if errorlevel 1 (
        echo failed to build the %PROJECT_NAME% driver's test-data DLL
        ctime.exe -end metrics\but.ctm %errorlevel%
        GOTO :EOF
    )

    IF %verbose% EQU 1 (
        ECHO.
        ECHO Build the %PROJECT_NAME% Driver
    )
    cl %CommonCompilerFlagsFinal% /I%DIR_INCLUDE% ^
    %DIR_REPO%\cmd\but\driver_windows.c  /Fo:%DIR_OUT_OBJ%\ ^
    /Fd:%DIR_OUT_BIN%\but.pdb /Fe:%DIR_OUT_BIN%\but.exe /link ^
    %CommonLinkerFlagsFinal% /ENTRY:mainCRTStartup
    if errorlevel 1 (
        echo failed to build the %PROJECT_NAME% Program
        ctime.exe -end metrics\but.ctm %errorlevel%
        GOTO :EOF
    )
)

ctime.exe -end metrics\but.ctm %errorlevel%

if %test% EQU 1 (
    if %verbose% EQU 1 (
        ECHO Run the %PROJECT_NAME% unit tests
        ECHO.
    )
    pushd %DIR_OUT_BIN%
    but.exe but_butts.dll
    popd
)
ENDLOCAL
