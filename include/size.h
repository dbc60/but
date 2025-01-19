#ifndef BUT_SIZE_H_
#define BUT_SIZE_H_

/**
 * @file size.h
 * @author Douglas Cuthbertson
 * @brief macros related to size_t.
 * @version 0.1.0
 * @date 2022-01-24
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <limits.h> // CHAR_BIT
#include <stddef.h> // size_t

#if defined(__cplusplus)
extern "C" {
#endif

#define KILO(V) ((V) * 1000LL)
#define MEGA(V) (KILO(V) * 1000LL)
#define GIGA(V) (MEGA(V) * 1000LL)
#define TERA(V) (GIGA(V) * 1000LL)

// Adopt ISO/IEC 80000 standard names for binary prefixes
#define KIBI(V) ((V) * 1024LL)
#define MEBI(V) (KIBI(V) * 1024LL)
#define GIBI(V) (MEBI(V) * 1024LL)
#define TEBI(V) (GIBI(V) * 1024LL)

/* ------------------- size_t and alignment properties -------------------- */

/* The byte and bit size of a size_t */
#define SIZE_T_SIZE    (sizeof(size_t))
#define SIZE_T_BITSIZE (sizeof(size_t) * CHAR_BIT)

/* Some constants coerced to size_t */
/* Annoying but necessary to avoid errors on some platforms */
#define SIZE_T_ZERO       ((size_t)0)
#define SIZE_T_ONE        ((size_t)1)
#define SIZE_T_TWO        ((size_t)2)
#define SIZE_T_FOUR       ((size_t)4)
#define TWO_SIZE_T_SIZES  (SIZE_T_SIZE << 1)
#define FOUR_SIZE_T_SIZES (SIZE_T_SIZE << 2)
#define SIX_SIZE_T_SIZES  (FOUR_SIZE_T_SIZES + TWO_SIZE_T_SIZES)
#define MAX_SIZE_T        (~(size_t)0)
#define HALF_MAX_SIZE_T   (MAX_SIZE_T >> 1)

#if defined(__cplusplus)
}
#endif

#endif // BUT_SIZE_H_
