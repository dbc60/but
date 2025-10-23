@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.
SET PROJECT_NAME="BUT Static Library"
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
    ctime.exe -begin metrics\but_lib.ctm
)

CALL %DIR_CMD%\setup.cmd %*

:: Build the static library
IF %build% EQU 1 (
    IF %verbose% EQU 1 (
        ECHO.
        ECHO Build the %PROJECT_NAME%
    )

    REM: Compile source files to object files
    cl %CommonCompilerFlagsFinal% /I"%DIR_INCLUDE%" /c ^
        %DIR_REPO%\src\exception.c ^
        %DIR_REPO%\src\exception_assert.c ^
        %DIR_REPO%\src\log.c ^
        /Fo:%DIR_OUT_OBJ%\ /Fd:%DIR_OUT_LIB%\but.pdb
    if errorlevel 1 (
        echo failed to compile %PROJECT_NAME% source files
        if %timed% EQU 1 (
            ctime.exe -end metrics\but_lib.ctm %errorlevel%
        )
        GOTO :EOF
    )

    REM: Link object files into static library
    lib /NOLOGO ^
        /OUT:%DIR_OUT_LIB%\but.lib ^
        %DIR_OUT_OBJ%\exception.obj ^
        %DIR_OUT_OBJ%\exception_assert.obj ^
        %DIR_OUT_OBJ%\log.obj
    if errorlevel 1 (
        echo failed to create %PROJECT_NAME%
        if %timed% EQU 1 (
            ctime.exe -end metrics\but_lib.ctm %errorlevel%
        )
        GOTO :EOF
    )

    REM: Copy public header files
    COPY %DIR_INCLUDE%\but.h %DIR_OUT_INC%\ 1>NUL
    COPY %DIR_INCLUDE%\but_assert.h %DIR_OUT_INC%\ 1>NUL
    COPY %DIR_INCLUDE%\but_macros.h %DIR_OUT_INC%\ 1>NUL
    COPY %DIR_INCLUDE%\exception.h %DIR_OUT_INC%\ 1>NUL
    COPY %DIR_INCLUDE%\exception_assert.h %DIR_OUT_INC%\ 1>NUL
    COPY %DIR_INCLUDE%\exception_types.h %DIR_OUT_INC%\ 1>NUL
    COPY %DIR_INCLUDE%\abbreviated_types.h %DIR_OUT_INC%\ 1>NUL
    COPY %DIR_INCLUDE%\intrinsics_win32.h %DIR_OUT_INC%\ 1>NUL
    if errorlevel 1 (
        echo failed to copy %PROJECT_NAME% header files to %DIR_OUT_INC%\
        if %timed% EQU 1 (
            ctime.exe -end metrics\but_lib.ctm %errorlevel%
        )
        GOTO :EOF
    )

    IF %verbose% EQU 1 (
        ECHO %PROJECT_NAME% built successfully: %DIR_OUT_LIB%\but.lib
    )
)

if %timed% EQU 1 (
    ctime.exe -end metrics\but_lib.ctm %errorlevel%
)

ENDLOCAL
