@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

:: /wd turns off a particular warning. The macros in the exception-handling code
:: will cause some local variables to nest, so we turn off this warning.
::
::   /wd4456 - warning C4456: declaration of '<var>' hides previous local declaration

SET PROJECT_NAME="Exceptions Library"
SET PROJECT_NAME=%PROJECT_NAME:"=%
TITLE %PROJECT_NAME%

:: Build the project
IF %build% EQU 1 (
    if NOT exist %DIR_OUT_OBJ%\exceptions md %DIR_OUT_OBJ%\exceptions
    if NOT exist %DIR_OUT_LIB%\exceptions md %DIR_OUT_LIB%\exceptions
    if NOT exist %DIR_OUT_BIN%\exceptions md %DIR_OUT_BIN%\exceptions
    if NOT exist %DIR_OUT_INC%\exceptions md %DIR_OUT_INC%\exceptions

    IF %verbose% EQU 1 (
        ECHO.
        ECHO Build the Exceptions library
    )

    cl %CommonCompilerFlagsFinal% %CommonCompilerFlagsBuildLIB% /c ^
    /I%DIR_INCLUDE% %DIR_REPO%\exceptions\exceptions.c ^
    %DIR_REPO%\exceptions\assert.c ^
    /Fo:%DIR_OUT_OBJ%\exceptions\ /Fd:%DIR_OUT_LIB%\exceptions\exceptions.pdb
    if errorlevel 1 (
        echo failed to compile the Exceptions library
        GOTO :EOF
    )

    lib %CommonLibrarianFlags% /subsystem:console %DIR_OUT_OBJ%\exceptions\exceptions.obj ^
        %DIR_OUT_OBJ%\exceptions\assert.obj /OUT:%DIR_OUT_LIB%\exceptions\exceptions.lib
    if errorlevel 1 (
        echo failed to link the Exceptions library
        GOTO :EOF
    )

    IF %verbose% EQU 1 (
        ECHO.
        ECHO Build the Exceptions test suite
    )
    cl %CommonCompilerFlagsFinal% %CommonCompilerFlagsBuildDLL% /wd4456 ^
    /I%DIR_INCLUDE% /DDLL_BUILD ^
    %DIR_REPO%\exceptions\butts.c ^
    /Fo:%DIR_OUT_OBJ%\exceptions\ /Fd:%DIR_OUT_LIB%\exceptions\butts.pdb ^
    /LD /link %CommonLinkerFlagsFinal% ^
    /OUT:%DIR_OUT_BIN%\exceptions\butts.dll %DIR_OUT_LIB%\exceptions\exceptions.lib ^
    /IMPLIB:%DIR_OUT_LIB%\exceptions\butts.lib
    if errorlevel 1 (
        echo failed to link the Exceptions test suite
        GOTO :EOF
    )

    REM: copy exported header files
    COPY %DIR_INCLUDE%\exceptions\* %DIR_OUT_INC%\exceptions\ 1>NUL
    if errorlevel 1 (
        echo failed to copy Exceptions header files to %DIR_OUT_INC%\exceptions\
        GOTO :EOF
    )
)
ENDLOCAL
