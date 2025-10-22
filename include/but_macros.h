#ifndef BUT_MACROS_H_
#define BUT_MACROS_H_

/**
 * @file but_macros.h
 * @author Douglas Cuthbertson
 * @brief Project-wide macro definitions.
 * @version 0.1
 * @date 2022-01-24
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <stddef.h> // offsetof

#if !defined(THREAD_LOCAL)
// Note that for Visual Studio 2022 to define __STDC_VERSION__, the compiler
// option
// "/std:cxx" must be applied where "cxx" is one of "c11" or "c17". Without
// this option, __STDC_VERSION__ is undefined.
//
// Hopefully one day there will be a C23-compliant version of VS, because that
// will mean that `thread_local` will be a standard keyword.

// THREAD_LOCAL can be applied to objects that have static storage duration,
// that is any object that is valid during the entire execution of the program,
// such as global data objects (both static and extern), local static objects,
// and static data members of classes. It specifically excludes objects on the
// call stack.
// The "thread_local" keyword was added to C as of C23. It was added to C++ in
// C++11. In the meantime, C has had "_Thread_local" since C11.
#if defined(__cplusplus)
#define THREAD_LOCAL thread_local
#else                                                        // !__cplusplus
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L // C23 or later
#define THREAD_LOCAL thread_local
#else // !(__STDC_VERSION__ && __STDC_VERSION__ >= 202311L)
#if defined(_WIN32) || defined(WIN32) // _Thread_local and thread_local are undefined.
#define THREAD_LOCAL __declspec(thread)
#else // !(_WIN32 || WIN32)
#define THREAD_LOCAL _Thread_local
#endif // _WIN32 || WIN32
#endif // __STDC_VERSION__ && __STDC_VERSION__ > 201710L
#endif // __cplusplus
#endif // THREAD_LOCAL

#if defined(__cplusplus)
extern "C" {
#endif

// Three macros that are intended for different usages of the static keyword. They can be
// nice to use, but are entirely optional.

/**
 * @brief BUT_INTERNAL applied to a function definition declares that the function
 * is local to the compilation unit.
 */
#define BUT_INTERNAL static

/**
 * @brief BUT_LOCAL applied to a variable inside a function declares that the
 * variable is persistent (it is not a stack variable) and is available only to the
 * function.
 */
#define BUT_LOCAL static

/**
 * @brief BUT_GLOBAL applied to a variable outside a function declares that the
 * variable is local to the compilation unit.
 */
#define BUT_GLOBAL static

/*
 * Stringize a value.
 *
 * Usage:
 *  #pragma message("DBG: Compiling " __FILE__)
 *  #pragma message ("DBG: " __FILE__ "(" BUT_STR(__LINE__) "): test")
 *  #pragma message("DBG: DLL_SPEC " BUT_STR(DLL_SPEC))
 *
 * Example Output:
 *  DBG: Compiling W:\faultline\but\driver.c
 *  DBG: W:\faultline\but\driver.c(23): test
 *  DBG: DLL_SPEC __declspec(dllexport)
 */
#define BUT_XSTR(x) #x
#define BUT_STR(x)  BUT_XSTR(x)

/**
 * @brief Explicitly note the intent of not using a function parameter.
 *
 * It can eliminate compiler warnings about unused parameters.
 */
#define BUT_UNUSED(P) ((void)(P))

/// The number of elements in a fixed-size array
#define BUT_ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

/**
 * @brief Calculate the address of a struct given the address of a field in the
 * struct, the struct's type, and the name of the field to which the address
 * points. This is equivalent of Microsoft's CONTAINING_RECORD and Linux's
 * container_of macros.
 */
#define BUT_CONTAINER(addr, type, member) \
    ((type *)(((char *)(addr)) - offsetof(type, member)))

/**
 * @brief Given a type and a member, BUT_MEMBER_SIZE calculates the size of the member in
 * bytes at compile time.
 */
#define BUT_MEMBER_SIZE(type, member) sizeof(((type *)0)->member)

#if defined(_WIN32) || defined(WIN32)
#define DLL_SPEC_EXPORT __declspec(dllexport)
#define DLL_SPEC_IMPORT __declspec(dllimport)
#if defined(DLL_BUILD)
#define DLL_SPEC __declspec(dllexport)
#else // DLL_BUILD
#define DLL_SPEC __declspec(dllimport)
#endif // DLL_BUILD
#if !defined STDCALL
#define STDCALL __stdcall
#endif // STDCALL
#if !defined CDECL
#define CDECL __cdecl
#endif // CDECL
#else  // _WIN32 || WIN32
#define DLL_SPEC
#define STDCALL
#define CDECL
#endif

#if defined(__cplusplus)
}
#endif

#endif // BUT_MACROS_H_
