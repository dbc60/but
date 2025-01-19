@ECHO off
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

:: When this script runs, it sets these environment variables:
::  VSSolution: the name of the subdirectory under "%DIR_REPO%\builds\" where
::              the Visual Studion solution file is located.

:: Set a variable for each default path
SET "BUILD_TOOLS_PATH_VS2017CE=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
SET "BUILD_TOOLS_PATH_VS2017Pro=C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build"
SET "BUILD_TOOLS_PATH_MSBuild2017=C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build"
SET "BUILD_TOOLS_PATH_VS2019CE=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
SET "BUILD_TOOLS_PATH_VS2019Pro=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build"
SET "BUILD_TOOLS_PATH_MSBuild2019=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build"
SET "BUILD_TOOLS_PATH_VS2022CE=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build"
SET "BUILD_TOOLS_PATH_VS2022Pro=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build"
SET "BUILD_TOOLS_PATH_MSBuild2022=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build"

:: Look for the most recent version of Visual Studio installed
if %vs2022% EQU 1 (
    if "%VSSOLUTION%"=="" (
        IF EXIST "%BUILD_TOOLS_PATH_MSBuild2022%" (
            SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH_MSBuild2022%"
            SET VSSOLUTION=vs2022
        ) ELSE IF EXIST "%BUILD_TOOLS_PATH_VS2022Pro%" (
            SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH_VS2022Pro%"
            SET VSSOLUTION=vs2022
        ) ELSE IF EXIST "%BUILD_TOOLS_PATH_VS2022CE%" (
            SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH_VS2022CE%"
            SET VSSOLUTION=vs2022
        )
    )
)

if %vs2019% EQU 1 (
    if "%VSSOLUTION%"=="" (
        IF EXIST "%BUILD_TOOLS_PATH_MSBuild2019%" (
            SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH_MSBuild2019%"
            SET VSSOLUTION=vs2019
        ) ELSE IF EXIST "%BUILD_TOOLS_PATH_VS2019Pro%" (
            SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH_VS2019Pro%"
            SET VSSOLUTION=vs2019
        ) ELSE IF EXIST "%BUILD_TOOLS_PATH_VS2019CE%" (
            SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH_VS2019CE%"
            SET VSSOLUTION=vs2019
        )
    )
)

if %vs2017% EQU 1 (
    if "%VSSOLUTION%"=="" (
        IF EXIST "%BUILD_TOOLS_PATH_MSBuild2017%" (
            SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH_MSBuild2017%"
            SET VSSOLUTION=vs2017
        ) ELSE IF EXIST "%BUILD_TOOLS_PATH_VS2017Pro%" (
            SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH_VS2017Pro%"
            SET VSSOLUTION=vs2017
        ) ELSE IF EXIST "%BUILD_TOOLS_PATH_VS2017CE%" (
            SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH_VS2017CE%"
            SET VSSOLUTION=vs2017
        )
    )
)

:: No known version of Visual Studio was found
IF "!BUILD_TOOLS_PATH!"=="" (
    ECHO Visual Studio is not installed, or is installed on an unexpected path.
    GOTO :EOF
)

:: CALL 'vcvars64.bat' for the selected build tool and check for errors.
IF NOT "!VSINSTALLDIR!" == "" GOTO :EOF
IF %x64% EQU 1 (
    CALL "%BUILD_TOOLS_PATH%\vcvars64.bat"
) ELSE IF %x86% EQU 1 (
    CALL "%BUILD_TOOLS_PATH%\vcvars32.bat"
)

IF "!VSINSTALLDIR!" == "" GOTO badenv

:: Set some variables in the shell
ENDLOCAL & (
    REM Project environment variables
    SET "BUILD_TOOLS_PATH=%BUILD_TOOLS_PATH%"
    SET "VSSOLUTION=%VSSOLUTION%"

    REM Visual Studio environment variables
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
GOTO :EOF

:badenv
ECHO VSINSTALLDIR is not defined.
ECHO.
ECHO Depending on your Visual Studio edition and install path one of these might work:
ECHO %comspec% /k "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
ECHO or
ECHO %comspec% /k "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsx86_amd64.bat"
ECHO.
