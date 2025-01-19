#ifndef CRY_MATH_WINDOWS_H_
#define CRY_MATH_WINDOWS_H_
/**
 * @file math_windows.h
 * @author Douglas Cuthbertson
 * @brief
 * @version 0.1
 * @date 2024-11-17
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <platform.h> // u16, u32, u64

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h> // _BitScanReverse

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Greatest power of 2 less than or equal to the given value
 *
 */
#define MATH_GREATEST_LOG2_BIT32(VAL, P2) \
    do {                                  \
        u32   v = (u32)(VAL);             \
        DWORD x;                          \
        _BitScanReverse(&x, v);           \
        (P2) = x;                         \
    } while (0)

#define MATH_GREATEST_LOG2_BIT64(VAL, P2) \
    do {                                  \
        u64   v = (VAL);                  \
        DWORD x;                          \
        _BitScanReverse64(&x, v);         \
        (P2) = x;                         \
    } while (0)



#if defined(__cplusplus)
}
#endif

#endif // CRY_MATH_WINDOWS_H_
