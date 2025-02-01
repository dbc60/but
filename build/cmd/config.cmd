@echo off
SETLOCAL ENABLEDELAYEDEXPANSION ENABLEEXTENSIONS

:: See LICENSE.txt for copyright and licensing information about this file.

:: Added _UNICODE and UNICODE so I can use Unicode strings in structs and
:: whatnot.
::
:: Need user32.lib to link MessageBox(), which es used on branch DAY001
:: Need gdi32.lib to link PatBlt(), which es used on branch DAY002
:: 2015.01.25 (Day004) I added /FC to get full path names in diagnostics. It's
:: helpful when using Emacs to code and launch the debugger and executable

:: /GS- turn off security checks because that compile-time option relies on
:: the C runtime library, which we are not using.

:: /Gs[size] The number of bytes that local variables can occupy before
:: a stack probe is initiated. If the /Gs option is specified without a
:: size argument, it is the same as specifying /Gs0

:: /Gm- disable minimal rebuild. We want to build everything. It won't
:: take long.

:: /GR- disable C++ RTTI. We don't need runtime type information.

:: /EHsc enable C++ EH (no SEH exceptions) (/EHs),
:: and  extern "C" defaults to nothrow (/EHc). That is, the compiler assumes
:: that functions declared as extern "C" never throw a C++ exception.

:: /EHa- disable C++ Exception Handling, so we don't have stack unwind code.
:: Casey says we don't need it.


:: /W3 set warning level 3.
:: /W4 set warning level 4. It's better
:: /WX warnings are errors
:: /wd turns off a particular warning
::   /wd4100 - 'identifier' : unreferenced formal parameter (this happens a lot while developing code)
::   /wd4189 - 'identifier' : local variable is initialized but not referenced
::   /wd4127 - conditional expression is constant ("do {...} while (0)" in macros)
::   /wd4456 - warning C4456: declaration of '<var>' hides previous local declaration
::   /wd6246 - Local declaration of '<var>' hides declaration of the same name in outer scope. This warning
::             occurs when the /analyze option is used. The exception code can cause nested blocks each
::             declaring exm_env_. This warning is not a problem.
::   /wd28301: No annotations for first declaration of "var"

:: TODO: add a build option to include /analyze. This is a static code analysis tool
SET ANALYZE_FLAGS= /analyze /wd6246 /wd28301

:: /FC use full pathnames in diagnostics

:: /Od - disable optimizations. The debug mode is good for development

:: /Oi Generate intrinsic functions. Replaces some function calls with
:: intrinsic or otherwise special forms of the function that help your
:: application run faster.

:: /GL whole program optimization. Use the /LTCG linker option to create the
:: output file. /ZI cannot be used with /GL. This is "enable link-time code generation"
:: in VS2022 and the /LTCG option has been removed.

:: /I<dir> add to include search path

:: /Fe:<file> name executable file

:: /D<name>{=|#}<text> define macro

:: /Zi enable debugging information
:: /Z7 enable debugging information

:: /link [linker options and libraries] The linker options are
:: documented here: https://msdn.microsoft.com/en-us/library/y0zzbyt4.aspx

:: /nodefaultlib t

:: Note that subsystem version number 5.1 only works with 32-bit builds.
:: The minimum subsystem version number for 64-bit buils is 5.2.
:: /subsystem:windows,5.1 - enable compatibility with Windows XP (5.1)

:: /LTCG link time code generation

:: /STACK:reserve[,commit] stack allocations. The /STACK option sets the size
:: of the stack in bytes. Use this option only when you build an .exe file.

:: DEFINITIONS
::   _UNICODE           - 16-bit wide characters
::   UNICODE            - 16-bit wide characters
::   BUILD_INTERNAL     - 0 = build for public release,
::                        1 = build for developers only
::   BUILD_SLOW         - 0 = No slow code (like assertion checks) allowed!,
::                        1 = Slow code welcome
::   __ISO_C_VISIBLE    - the version of C we are targeting for the math library.
::                        1995 = C95, 1999 = C99, 2011 = C11.

:: BUILD PROPERTIES
:: It's possible to set build properties from the command line using the /p:<Property>=<value>
:: command-line option. For example, to set TargetPlatformVersion to 10.0.10240.0, you would
:: add "/p:TargetPlatformVersion=10.0.10240.0" and possibly
:: "/p:WindowsTargetPlatformVersion=10.0.10240.0". Note that the TargetPlatformVersion setting
:: is optional and allows you to specify the kit version to build with. The default is to use
:: the latest kit.

:: Building Software Using the Universal CRT (VS2015)
:: Use the UniversalCRT_IncludePath property to find the Universal CRT SDK header files.
:: Use one of the following properties to find the linker/library files:
::    UniversalCRT_LibraryPath_x86
::    UniversalCRT_LibraryPath_x64
::    UniversalCRT_LibraryPath_arm

:: Common compiler flags
:: /nologo don't display the logo
:: /Zc:
::  wchar_t     wchar_t is the native type, not a typedef
::  forScope    enforce Standard C++ for scoping rules
::  inline      remove unreferenced function or data if it is COMDAT or has
::              internal linkage only (off by default)
:: /Gd      __cdecl calling convention
:: /Gm-     disable minimal rebuild
:: /GR-     disable C++ RTTI
:: /EHa-    disable C++ EH (w/ SEH exceptions)
:: /EHsc    enable C++ EH, extern "C" defaults to nothrow
:: /std:c11 enable C11 standard
:: /std:c17 enable C17 standard (a bug fix for C11)
:: /experimental:c11atomics enable C11/C17 atomics
:: /Oi      enable intrinsic functions
:: /WX      treat warnings as errors
:: /W4      set warning level 4
:: /FC      use full pathnames in diagnostics
:: /DNAME   define a macro called NAME
SET CommonCompilerFlags=/nologo /Zc:wchar_t,forScope,inline /Gd /Gm- /GR- /EHa- /EHsc ^
    /std:c17 /experimental:c11atomics /Oi /WX /W4 /volatile:iso /wd4127 /FC /D_UNICODE ^
    /DUNICODE /D_WIN32 /DWIN32

::SET CStandardLibraryIncludeFlags=/I"%VSINSTALLDIR%SDK\ScopeCppSDK\SDK\include\ucrt"
::SET CMicrosoftIncludeFlags=/I"%VSINSTALLDIR%SDK\ScopeCppSDK\SDK\include\um" ^
::    /I"%VSINSTALLDIR%SDK\ScopeCppSDK\SDK\include\shared"
::SET CRuntimeIncludeFlags=/I"%VSINSTALLDIR%SDK\ScopeCppSDK\VC\include"

:: Debug and optimized compiler flags
:: /MTd     link with LIBCMTD.LIB debug lib
:: /Zi      enable debugging information
:: /Od      disable optimizations (default)
SET CommonCompilerFlagsDEBUG=/MTd /Zi /Od /DDEBUG %CommonCompilerFlags%

:: /MT link with LIBCMT.LIB
:: /GL enable link-time code generation
:: /Zo generate richer debugging information for optimized code (on by default)
:: /O2 maximum optimizations (favor speed)
:: /Oi enable intrinsic functions
:: favor:<blend|ATOM> select processor to optimize for, one of:
::     blend - a combination of optimizations for several different x86 processors
::     ATOM - Intel(R) Atom(TM) processors
SET CommonCompilerFlagsOPTIMIZE=/MT /GL /O2 /Oi /favor:blend %CommonCompilerFlags%

:: Preprocessor definitions for a Library build
SET CommonCompilerFlagsBuildLIB=/D_LIB

:: Per https://learn.microsoft.com/en-us/cpp/build/regular-dlls-statically-linked-to-mfc?view=msvc-170
:: Even though the term USRDLL is obsolete, you must still define "_USRDLL" on the
:: compiler command line. This definition determines which declarations is pulled in from
:: the MFC header files.
::
:: Per https://learn.microsoft.com/en-us/cpp/mfc/tn011-using-mfc-as-part-of-a-dll?view=msvc-170
::
::  When compiling regular MFC DLLs that statically link to MFC, the symbols _USRDLL and
::  _WINDLL must be defined. Your DLL code must also be compiled with the following compiler switches:
::
::      /D_WINDLL signifies the compilation is for a DLL
::      /D_USRDLL signifies the compilation is for a regular MFC DLL
::
:: I don't build MFC DLLs, so I don't need to define _WINDLL. I do need to define _USRDLL.
SET CommonCompilerFlagsBuildMFC=

:: Choose either Debug or Optimized Compiler Flags
IF %release% EQU 1 (
    SET CommonCompilerFlagsFinal=%CommonCompilerFlagsOPTIMIZE%
) ELSE (
    SET CommonCompilerFlagsFinal=%CommonCompilerFlagsDEBUG%
)


:: Common linker flags
:: set CommonLinkerFlags=/incremental:no /opt:ref user32.lib gdi32.lib winmm.lib
SET CommonLinkerFlags=/nologo /incremental:no /MANIFESTUAC /incremental:no /opt:ref
SET CommonLinkerFlagsX64=/MACHINE:X64 %CommonLinkerFlags%
SET CommonLinkerFlagsX86=/MACHINE:X86 %CommonLinkerFlags%

:: Choose 32-bit or 64-bit build
if %x86% EQU 1 (
    SET CommonLinkerFlagsFinal=%CommonLinkerFlagsX86%
) else if %x64% EQU 1 (
    SET CommonLinkerFlagsFinal=%CommonLinkerFlagsX64%
) else (
    ECHO CONFIG.CMD ERROR: Unknown platform target "%PLATFORM%"
    GOTO :EOF
)

:: Visual Studio Librarian Options
if %debug% EQU 1 (
    SET CommonLibrarianFlags=/nologo
)

:: "LTCG" stands for link-time code generation.
if %release EQU 1 (
    SET CommonLibrarianFlags=/LTCG /nologo
)

if %trace% EQU 1 (
    ECHO CommonCompilerFlagsFinal = %CommonCompilerFlagsFinal%
    ECHO CommonLinkerFlagsFinal = %CommonLinkerFlagsFinal%
    ECHO CommonLibrarianFlags = %CommonLibrarianFlags%
)

:: It seems that the minimum subsystem is 5.02 for 64-bit Windows XP. Both "/subsystem:windows,5.1" and
:: /subsystem:windows,5.01" failed with linker warning "LNK4010: invalid subsystem version number 5.1"

:: 32-bit build
:: cl %CommonCompilerFlags% "%DIR_REPO%\src\win32_all.cpp" /link /subsystem:windows,5.02 %CommonLinkerFlagsFinal%

:: 64-bit build
:: Optimization switches /O2 /Oi /fp:fast

ENDLOCAL & (
    SET "CommonCompilerFlagsFinal=%CommonCompilerFlagsFinal%"
    SET "CommonLinkerFlagsFinal=%CommonLinkerFlagsFinal%"
    SET "CommonLibrarianFlags=%CommonLibrarianFlags%"
    SET "CommonCompilerFlagsBuildMFC=%CommonCompilerFlagsBuildMFC%"
)
