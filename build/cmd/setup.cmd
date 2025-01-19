@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

SET DIR_CMD=%~dp0
SET DIR_CMD=%DIR_CMD:~0,-1%
SET DIR_LOCAL=%DIR_CMD%\local
FOR /f "delims=" %%F IN ("%DIR_CMD%") DO (
    SET DIR_BUILD=%%~dpF
)
SET DIR_BUILD=%DIR_BUILD:~0,-1%

FOR /f "delims=" %%F IN ("%DIR_BUILD%") DO (
    SET DIR_REPO=%%~dpF
)
SET DIR_REPO=%DIR_REPO:~0,-1%

FOR /F "delims=" %%F IN ("%DIR_REPO%") DO (
    SET REPO_NAME=%%~nF
)

:: If "build" is not set, then set it if test is set or if neither clean nor
:: cleanall are set
if %build% EQU 0 (
    if %test% EQU 1 (
        SET build=1
    )
    if %cleanall% EQU 0 (
        if %clean% EQU 0 (
            if %cleanplat% EQU 0 (
                SET build=1
            )
        )
    )
)

:: Make sure we'll look for at least one version of Visual Studio
if %vs2022% EQU 0 (
    if %vs2019% EQU 0 (
        if %vs2017% EQU 0 (
            REM None are set, so try all of them
            SET vs2022=1
            SET vs2019=1
            SET vs2017=1
        )
    )
)

:: If there's no indication of building for a 64-bit or 32-bit platform, then
:: default to a 64-bit platform.
if %x64% EQU 0 (
    if %x86% EQU 0 (
        if %win32% EQU 0 (
            SET x64=1
        )
    )
)

if %x64% EQU 1 (
    SET DIR_PLATFORM=x64
) else if %x86% EQU 1 (
    SET DIR_PLATFORM=x86
) else if %win32% EQU 1 (
    REM Check only x86 instead of both win32 and x86
    SET x86=1
    SET DIR_PLATFORM=x86
)

:: If neither debug nor release have been selected, choose debug
if %debug% EQU 0 (
    if %release% EQU 0 (
        SET debug=1
    )
)

if %trace% EQU 1 (
    FOR %%O in (%options%) DO FOR /f "tokens=1,* delims=:" %%A in ("%%O") DO (
        ECHO OPTION: %%A = !%%A!
    )
)

SET DIR_TARGET=target
if %debug% EQU 1 (
    SET BUILD_TYPE=debug
) else (
    SET BUILD_TYPE=release
)

if %build% EQU 1 (
    IF "%VSSOLUTION%"=="" (
        CALL !DIR_CMD!\shell.cmd >NUL
    )
)

:: Make sure VSSOLUTION is set in case clean or cleanall are set.
if "%VSSOLUTION%"=="" (
    if %vs2022% EQU 1 (
        SET VSSOLUTION=vs2022
    ) else if %vs2019% EQU 1 (
        SET VSSOLUTION=vs2019
    ) else if %vs2017% EQU 1 (
        SET VSSOLUTION=vs2017
    )
)

if %build% EQU 1 (
    CALL !DIR_CMD!\config.cmd
)

:: Build artifacts are written in the current repository to the path
:: DIR_TARGET\Compiler\Platform\BuildType, where
::
::  - Compiler is one of vs2017, vs2019, or vs2022.
::  - Platform is either x64 or x86.
::  - BuildType is either debug or release.
::
:: For example build artifacts for a 64-bit debug-build created with Visual
:: Studio 2022 woule be written to "build\vs2022\x64\debug\".
SET DIR_OUT_BASE=%DIR_REPO%\%DIR_TARGET%\%VSSOLUTION%
SET DIR_OUT_PLATFORM=!DIR_OUT_BASE!\!DIR_PLATFORM!
SET DIR_OUT_BUILD=!DIR_OUT_PLATFORM!\%BUILD_TYPE%
SET DIR_OUT_OBJ=!DIR_OUT_BUILD!\obj
SET DIR_OUT_LIB=!DIR_OUT_BUILD!\lib
SET DIR_OUT_BIN=!DIR_OUT_BUILD!\bin
SET DIR_OUT_INC=!DIR_OUT_BUILD!\inc
SET DIR_INCLUDE=%DIR_REPO%\include

if %trace% EQU 1 (
    ECHO SET_ENV.CMD: DIR_INCLUDE_PRIVATE   =!DIR_INCLUDE_PRIVATE!
    ECHO SET_ENV.CMD: DIR_INCLUDE           =!DIR_INCLUDE!
    ECHO SET_ENV.CMD: DIR_OUT_BASE          =!DIR_OUT_BASE!
    ECHO SET_ENV.CMD: DIR_OUT_PLATFORM      =!DIR_OUT_PLATFORM!
    ECHO SET_ENV.CMD: DIR_OUT_BUILD         =!DIR_OUT_BUILD!
    ECHO SET_ENV.CMD: DIR_OUT_OBJ           =%DIR_OUT_OBJ%
    ECHO SET_ENV.CMD: DIR_OUT_LIB           =%DIR_OUT_LIB%
    ECHO SET_ENV.CMD: DIR_OUT_BIN           =%DIR_OUT_BIN%
)

:: Delete the build artifacts from all configurations of the VS build folder
IF %cleanall% EQU 1 (
    if %verbose% EQU 1 (
        IF EXIST %DIR_OUT_BASE% ECHO Clean All: deleting directory: %DIR_OUT_BASE%
    )
    IF EXIST %DIR_OUT_BASE% RD /S /Q %DIR_OUT_BASE%
    SET VSSOLUTION=
)

:: Delete the artifacts from the platform configuration (32-bit/64-bit). This includes
:: both the debug and release versions.
IF %cleanplat% EQU 1 (
    if %verbose% EQU 1 (
        IF EXIST %DIR_OUT_PLATFORM% ECHO Clean: deleting directory: %DIR_OUT_PLATFORM%
    )
    IF EXIST %DIR_OUT_PLATFORM% RD /S /Q %DIR_OUT_PLATFORM%
    SET VSSOLUTION=
)

:: Delete the artifacts from the current build type (debug/release)
IF %clean% EQU 1 (
    if %verbose% EQU 1 (
        IF EXIST %DIR_OUT_BUILD% ECHO Clean Build: deleting directory: %DIR_OUT_BUILD%
    )
    IF EXIST %DIR_OUT_BUILD% RD /S /Q %DIR_OUT_BUILD%
    SET VSSOLUTION=
)

:: Create the build directory if we're building
IF !build!==1 (
    IF NOT EXIST %DIR_OUT_BIN% MD %DIR_OUT_BIN%
    IF NOT EXIST %DIR_OUT_LIB% MD %DIR_OUT_LIB%
    IF NOT EXIST %DIR_OUT_OBJ% MD %DIR_OUT_OBJ%
    IF NOT EXIST %DIR_OUT_INC% MD %DIR_OUT_INC%
)

:: Return Variables
ENDLOCAL & (
    REM A flag to indicate that setup has been called

    REM Environment variables from config.cmd
    SET "CommonCompilerFlagsFinal=%CommonCompilerFlagsFinal%"
    SET "CommonLinkerFlagsFinal=%CommonLinkerFlagsFinal%"
    SET "CommonLibrarianFlags=%CommonLibrarianFlags%"

    REM Export all command-line options except for win32
    SET "options=%options%"
    SET "build=%build%"
    SET "debug=%debug%"
    SET "release=%release%"
    SET "clean=%clean%"
    SET "cleanplat=%cleanplat%"
    SET "cleanall=%cleanall%"
    SET "x64=%x64%"
    SET "x86=%x86%"
    SET "test=%test%"
    SET "vs2017=%vs2017%"
    SET "vs2019=%vs2019%"
    SET "vs2022=%vs2022%"
    SET "verbose=%verbose%"
    SET "trace=%trace%"

    REM Local environment variables
    SET "DIR_REPO=%DIR_REPO%"
    SET "DIR_TARGET=%DIR_TARGET%"
    SET "BUILD_TYPE=%BUILD_TYPE%"
    SET "DIR_INCLUDE=%DIR_INCLUDE%"
    SET "DIR_INCLUDE_PRIVATE=%DIR_INCLUDE_PRIVATE%"
    SET "DIR_OUT_OBJ=%DIR_OUT_OBJ%"
    SET "DIR_OUT_LIB=%DIR_OUT_LIB%"
    SET "DIR_OUT_BIN=%DIR_OUT_BIN%"
    SET "VSSOLUTION=%VSSOLUTION%"
    SET "DIR_OUT_INC=%DIR_OUT_INC%"

    SET "VSINSTALLDIR=%VSINSTALLDIR%"
    SET "PATH=%PATH%"
    SET "PLATFORM=%PLATFORM%"
    SET "INCLUDE=%INCLUDE%"
    SET "LIB=%LIB%"
    SET "LIBPATH=%LIBPATH%"
    SET "UCRTVersion=%UCRTVersion%"
    SET "UniversalCRTSdkDir=%UniversalCRTSdkDir%"
    SET "VCIDEInstallDir=%VCIDEInstallDir%"
    SET "VCINSTALLDIR=%VCINSTALLDIR%"
    SET "VCToolsInstallDir=%VCToolsInstallDir%"
    SET "VCToolsRedistDir=%VCToolsRedistDir%"
    SET "VCToolsVersion=%VCToolsVersion%"
    SET "VisualStudioVersion=%VisualStudioVersion%"
    SET "VS150COMNTOOLS=%VS150COMNTOOLS%"
    SET "VS160COMNTOOLS=%VS160COMNTOOLS%"
    SET "WindowsLibPath=%WindowsLibPath%"
    SET "WindowsSdkBinPath=%WindowsSdkBinPath%"
    SET "WindowsSdkDir=%WindowsSdkDir%"
    SET "WindowsSDKLibVersion=%WindowsSDKLibVersion%"
    SET "WindowsSdkVerBinPath=%WindowsSdkVerBinPath%"
    SET "WindowsSDKVersion=%WindowsSDKVersion%"
    SET "DevEnvDir=%DevEnvDir%"
    SET "ExtensionSdkDir=%ExtensionSdkDir%"
    SET "Framework40Version=%Framework40Version%"
    SET "FrameworkDir=%FrameworkDir%"
    SET "FrameworkDir64=%FrameworkDir64%"
    SET "FrameworkDir32=%FrameworkDir32%"
    SET "FrameworkVersion=%FrameworkVersion%"
    SET "FrameworkVersion64=%FrameworkVersion64%"
    SET "FrameworkVersion32=%FrameworkVersion32%"
)
