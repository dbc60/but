#ifndef BUT_INTRINSICS_WINDOWS_H_
#define BUT_INTRINSICS_WINDOWS_H_

/**
 * @file intrinsics.h
 * @author Douglas Cuthbertson
 * @brief declare intrinsic functions w/o linking to the C runtime
 * @version 0.1
 * @date 2023-03-12
 *
 * There are several functions that are designated as intrinsic (see
 * https://msdn.microsoft.com/en-us/library/tzkfha43.aspx). All intrinsic
 * functions are inline, so they do not generate a function call. These are
 * also part of the C runtime library, so to eliminate the C runtime library,
 * one must replace these intrinsics. The problem is that in a Release build
 * the compiler will complain that you're trying to replace an intrinsic
 * function. The solution is to declare the function in a header with a pragma
 *
 *      #pragma intrinsic(function-name)
 *
 * and use another pragma (see
 * https://msdn.microsoft.com/en-us/library/sctyh01s.aspx) and
 * src/intrinsics_windows.c when the function is defined. Below, we have a
 * pragma declaration for memset. Add other declarations and definitions as
 * needed.
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <project.h>

#include <stddef.h> // size_t

#if defined(__cplusplus)
extern "C" {
#endif

// disable warning C28251: Inconsistent annotation for 'memcpy': this instance has no
// annotations.
// #pragma warning(disable : 28251) // for when /analyze is on

void *CDECL memcpy(void *restrict dst, const void *restrict src, size_t n);
#pragma intrinsic(memcpy)

void *CDECL memset(void *, int, size_t);
#pragma intrinsic(memset)

#if defined(__cplusplus)
}
#endif

#endif // BUT_INTRINSICS_WINDOWS_H_
