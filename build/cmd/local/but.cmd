@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: Build the Basic Unit Test (BUT) libraries.
::
:: butts.dll: test suite for testing BUT functionality.
:: test_data.dll: test data for butts.dll.
:: but.exe: the BUT test driver.

:: See LICENSE.txt for copyright and licensing information about this file.

SET PROJECT_NAME="Basic Unit Test Program"
SET PROJECT_NAME=%PROJECT_NAME:"=%
TITLE %PROJECT_NAME%

:: Build the project
IF %build% EQU 1 (
    IF NOT exist %DIR_OUT_OBJ%\but md %DIR_OUT_OBJ%\but
    IF NOT exist %DIR_OUT_LIB%\but md %DIR_OUT_LIB%\but
    IF NOT exist %DIR_OUT_BIN%\but md %DIR_OUT_BIN%\but
    IF NOT exist %DIR_OUT_INC%\but md %DIR_OUT_INC%\but

    if %verbose% EQU 1 (
        ECHO.
        ECHO Build the BUT driver test suite
    )
    cl %CommonCompilerFlagsFinal% /I"%DIR_INCLUDE%" /DDLL_BUILD ^
    %DIR_REPO%\but\context.c %DIR_REPO%\but\test.c %DIR_REPO%\but\butts.c ^
    /Fo:%DIR_OUT_OBJ%\but\ /Fd:%DIR_OUT_BIN%\but\butts.pdb ^
    /LD /link %CommonLinkerFlagsFinal% /LIBPATH:%DIR_OUT_LIB% ^
    /OUT:%DIR_OUT_BIN%\but\butts.dll %DIR_OUT_LIB%\exceptions\exceptions.lib ^
    /IMPLIB:%DIR_OUT_LIB%\but\butts.lib
    if errorlevel 1 (
        echo failed to build the BUT driver test suite
        GOTO :EOF
    )

    if %verbose% EQU 1 (
        ECHO.
        ECHO Build the BUT driver test-data DLL
    )
    cl %CommonCompilerFlagsFinal% /I"%DIR_INCLUDE%" /DDLL_BUILD ^
    %DIR_REPO%\but\test_data.c ^
    /Fo:%DIR_OUT_OBJ%\but\ ^
    /Fd:%DIR_OUT_BIN%\but\test_data.pdb ^
    /LD /link %CommonLinkerFlagsFinal% ^
    /OUT:%DIR_OUT_BIN%\but\test_data.dll ^
    %DIR_OUT_OBJ%\but\context.obj ^
    %DIR_OUT_LIB%\exceptions\exceptions.lib ^
    /IMPLIB:%DIR_OUT_LIB%\but\test_data.lib
    if errorlevel 1 (
        echo failed to build the BUT driver test-data DLL
        GOTO :EOF
    )

    IF %verbose% EQU 1 (
        ECHO.
        ECHO Build the Basic Unit Test Program
    )
    cl %CommonCompilerFlagsFinal% ^
    /I%DIR_INCLUDE% ^
    %DIR_REPO%\cmd\but\driver.c  /Fo:%DIR_OUT_OBJ%\but\ ^
    /Fd:%DIR_OUT_BIN%\but\driver.pdb /Fe:%DIR_OUT_BIN%\but\but.exe /link ^
    %CommonLinkerFlagsFinal% /ENTRY:mainCRTStartup ^
    %DIR_OUT_OBJ%\but\context.obj ^
    %DIR_OUT_LIB%\exceptions\exceptions.lib
    if errorlevel 1 (
        echo failed to build the Basic Unit Test Program
        GOTO :EOF
    )
)
ENDLOCAL
