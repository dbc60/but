@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.
SET PROJECT_NAME="Exceptions Module"
SET PROJECT_NAME=%PROJECT_NAME:"=%
TITLE %PROJECT_NAME%

SET DIR_CMD=%~dp0
SET DIR_CMD=%DIR_CMD:~0,-1%
SET DIR_LOCAL=%DIR_CMD%\local
CALL %DIR_CMD%\options.cmd %*

if %timed% EQU 1 (
    if NOT EXIST metrics (
        md metrics
    )
    ctime.exe -begin metrics\exception.ctm
)

CALL %DIR_CMD%\setup.cmd %*

:: The build defaults to debug unless release is explicitly passed in
IF NOT "%release%"=="" (
    if %release% EQU 1 (
        SET REL_OPT=release
    )
)

:: Build the project
IF %build% EQU 1 (
    IF %verbose% EQU 1 (
        ECHO.
        ECHO Build the %PROJECT_NAME% test suite
    )
    cl %CommonCompilerFlagsFinal% /wd4456 /I%DIR_INCLUDE% /DDLL_BUILD ^
    %DIR_REPO%\src\exception_butts.c ^
    /Fo:%DIR_OUT_OBJ%\ /Fd:%DIR_OUT_LIB%\exception_butts.pdb ^
    /LD /link %CommonLinkerFlagsFinal% ^
    /OUT:%DIR_OUT_BIN%\exception_butts.dll ^
    /IMPLIB:%DIR_OUT_LIB%\exception_butts.lib
    if errorlevel 1 (
        echo failed to build the %PROJECT_NAME% test suite
        if %timed% EQU 1 (
            ctime.exe -end metrics\exception.ctm %errorlevel%
        )
        GOTO :EOF
    )

    REM: copy exported header files
    COPY %DIR_INCLUDE%\exception* %DIR_OUT_INC%\ 1>NUL
    if errorlevel 1 (
        echo failed to copy Exceptions header files to %DIR_OUT_INC%\
        if %timed% EQU 1 (
            ctime.exe -end metrics\exception.ctm %errorlevel%
        )
        GOTO :EOF
    )
)

if %timed% EQU 1 (
    ctime.exe -end metrics\exception.ctm %errorlevel%
)

if %test% EQU 1 (
    IF NOT EXIST "%DIR_OUT_BIN%\but.exe" (
        call %DIR_CMD%\but.cmd build %REL_OPT%
        if errorlevel 1 (
            GOTO :EOF
        )
    )
)

if %test% EQU 1 (
    if %verbose% EQU 1 (
        ECHO Run the %PROJECT_NAME% unit tests
        ECHO.
    )
    pushd %DIR_OUT_BIN%
    but.exe exception_butts.dll
    popd
)
ENDLOCAL
