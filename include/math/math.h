#ifndef CRY_MATH_H_
#define CRY_MATH_H_

/**
 * @file math.h
 * @author Douglas Cuthbertson
 * @brief The Number of Leading Zeros (NLZ) implementations are inspired by
 * Warren Jr, H. S. (2002). Hacker's Delight (pp. 77-80). Addison-Wesley.
 * @version 0.1
 * @date 2022-02-12
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#if defined(_MSC_VER) && _MSC_VER >= 1300
#include "math_windows.h"
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
// #include "math_linux.h"
#error math_linux.h is not yet implemented
#elif defined(__INTEL_COMPILER)
// #include "math_intel.h"
#error math_intel.h is not yet implemented
#else
// #include "math_generic.h"
#error math_generic.h is not yet implemented
#endif

#include <platform.h> // u16, u32, u64

#if defined(__cplusplus)
extern "C" {
#endif

#define SUM_OVER_SCALED_RANGE(LO, HI, SCALE, SZ)                           \
    do {                                                                   \
        u32 SR = (u32)((HI) - (LO)) / (SCALE) + 1;                         \
        (SZ)   = (SR % 2) ? (((LO) + ((HI) - (SCALE))) * (SR >> 1)) + (HI) \
                          : ((LO) + (HI)) * (SR >> 1);                     \
    } while (0)

/**
 * @brief Count the NLZ in a 16-bit unsigned integer.
 *
 * @param val
 * @return the number of leading zeros in val.
 */
u16 math_count_leading_zeros16(u16 val);

/**
 * @brief  Count the NLZ in a 32-bit unsigned integer.
 *
 * @param val calculate the NLZ of this value.
 * @return the number of leading zeros in val.
 */
u16 math_count_leading_zeros32(u32 val);

/**
 * @brief Count the NLZ in a 64-bit unsigned integer.
 *
 * @param val calculate the NLZ of this value.
 * @return the number of leading zeros in val.
 */
u16 math_count_leading_zeros64(u64 val);

/**
 * @brief Calculate the 16-bit base-2 log of a 16-bit value.
 *
 * @param val calculate the base-2 log from this value.
 * @return log2(val). If val == zero, the return value is invalid.
 */
u16 math_log2_bit16(u16 val);

/**
 * @brief Calculate the 16-bit base-2 log of a 32-bit value.
 *
 * @param val calculate the base-2 log from this value.
 * @return log2(val). If val == zero, the return value is invalid.
 */
u16 math_log2_bit32(u32 val);

/**
 * @brief Calculate the 16-bit base-2 log of a 64-bit value.
 *
 * @param val calculate the base-2 log from this value.
 * @return log2(val). If val == zero, the return value is invalid.
 */
u16 math_log2_bit64(u64 val);

/**
 * @brief Population count - count the number of ones (the population) in an
 * unsigned 32-bit integer.
 *
 * @param val calculate the population count from this value.
 * @return the population count of val.
 */
u16 math_pop_count32(u32 val);

/**
 * @brief Population count - count the number of ones (the population) in an
 * unsigned 64-bit integer.
 *
 * @param val calculate the population count from this value.
 * @return the population count of val.
 */
u16 math_pop_count64(u64 val);

#if defined(__cplusplus)
}
#endif

#endif // CRY_MATH_H_
