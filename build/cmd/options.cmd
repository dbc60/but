@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

:: Available options are:
::
::  build:      build the project. May be combined with release or debug.
::              Default is debug.
::  debug:      build without optimizations (default).
::  release:    build with optimizations.
::  cleanall:   delete all build artifacts for the configured compiler.
::  clean:      delete all build artifacts for the debug/release configuration.
::  cleanplat:  delete all build artifacts for the 32-bit/64-bit configuration.
::  x64:        specifies a 64-bit build (default).
::  x86:        specifies a 32-bit build.
::  win32:      specifies a 32-bit build.
::  test:       build the current configuration and run all unit tests.
::  vs2017:     use Visual Studio 2017. The scripts will search for MS Build,
::              Pro, and Community Edition in that order.
::  vs2019:     use Visual Studio 2019. The scripts will search for MS Build,
::              Pro, and Community Edition in that order.
::  vs2022:     use Visual Studio 2022. The scripts will search for MS Build,
::              Pro, and Community Edition in that order.
::  verbose:    Display details of steps during the build process.
::  trace:      Display the values of these options.

:: Remember to export these in the ENDLOCAL section below
SET "options=build: debug: release: cleanall: clean: cleanplat: x64: x86: win32: test: vs2017: vs2019: vs2022: verbose: trace:"
:: Initialize flags to zero
FOR %%O in (%options%) DO FOR /f "tokens=1,* delims=:" %%A in ("%%O") DO (
    if NOT "%%~B"=="" (
        SET "%%A=%%~B"
    ) else (
        SET "%%A=0"
    )
)

:loop
:: Validate and store the options, one at a time, using a loop.
:: Options start at arg 3 in this example. Each SHIFT is done starting at
:: the first option so required args are preserved.
::
if not "%~1"=="" (
    set "opt=!options:*%~1:=! "
    if "!opt!"=="!options! " (
        rem No substitution was made so this is an invalid option.
        rem Error handling goes here.
        rem I will simply echo an error message.
        echo Error: Invalid option %~1
    ) else if "!opt:~0,1!"==" " (
        rem Set the flag option using the option name.
        rem The value is 1, because my other scripts expect numeric values and
        rem are treating 1 and 0 like true and false.
        set "%~1=1"
    ) else (
        rem Set the option value using the option as the name.
        rem and the next arg as the value
        rem disable delayed expansion so that ! and ^ are preserved in option values.
        setlocal disableDelayedExpansion
        set "val=%~2"
        call :escapeVal
        setlocal enableDelayedExpansion
        if "!val!"=="^=^^" (
            REM The option was set to "", so unset it
            endlocal & endlocal & set "%~1="
        ) else (
            for /f delims^=^ eol^= %%A in ("!val!") do endlocal & endlocal & set "%~1=%%A" !
        )
        shift
    )
    shift
    goto :loop
    REM goto :EOF
)
goto :endArgs

:escapeVal
set "val=%val:^=^^%"
set "val=%val:!=^!%"
exit /b

:endArgs
if %trace% EQU 1 (
    ECHO OPTIONS.CMD: options SET
    FOR %%O in (%options%) DO FOR /f "tokens=1,* delims=:" %%A in ("%%O") DO (
        ECHO %%A = !%%A!
    )
    ECHO.
)

ENDLOCAL & (
    SET "options=%options%"
    SET "build=%build%"
    SET "debug=%debug%"
    SET "release=%release%"
    SET "clean=%clean%"
    SET "cleanplat=%cleanplat%"
    SET "cleanall=%cleanall%"
    SET "x64=%x64%"
    SET "x86=%x86%"
    SET "win32=%win32%"
    SET "test=%test%"
    SET "vs2017=%vs2017%"
    SET "vs2019=%vs2019%"
    SET "vs2022=%vs2022%"
    SET "verbose=%verbose%"
    SET "trace=%trace%"
)
