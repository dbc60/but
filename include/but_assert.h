#ifndef BUT_ASSERT_H_
#define BUT_ASSERT_H_

/**
 * @file but_assert.h
 * @author Douglas Cuthbertson
 * @brief Assertion macros for Basic Unit Test (BUT) framework
 * @version 0.1
 * @date 2025-06-26
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <exception.h> // BUT_THROW_DETAILS, but_internal_error

#include <inttypes.h> // PRIu32, PRId64, etc.
#include <math.h>     // fabs
#include <string.h>   // strcmp, memcmp
#include <stddef.h>   // size_t, NULL

#if defined(__cplusplus)
extern "C" {
#endif

// Helper macro to format assertion failure messages
#define BUT_ASSERT_IMPL(msg, ...) BUT_THROW_DETAILS(but_internal_error, msg, ##__VA_ARGS__)

// Unconditional failure
#define BUT_FAIL(msg, ...) BUT_ASSERT_IMPL("Failure: " msg, ##__VA_ARGS__)

// Boolean assertions
#define BUT_ASSERT_TRUE(expr)                                             \
    do {                                                                  \
        if (!(expr)) {                                                    \
            BUT_ASSERT_IMPL("Expected: true: %s. Actual: false.", #expr); \
        }                                                                 \
    } while (0)

#define BUT_ASSERT_FALSE(expr)                                            \
    do {                                                                  \
        if (expr) {                                                       \
            BUT_ASSERT_IMPL("Expected: false: %s. Actual: true.", #expr); \
        }                                                                 \
    } while (0)

// Integer comparisons
#define BUT_ASSERT_ZERO(value)                                      \
    do {                                                            \
        if ((value) != 0) {                                         \
            BUT_ASSERT_IMPL("Expected: zero. Actual: %d", (value)); \
        }                                                           \
    } while (0)

#define BUT_ASSERT_NON_ZERO(value)                                      \
    do {                                                                \
        if ((value) == 0) {                                             \
            BUT_ASSERT_IMPL("Expected: non-zero. Actual: %d", (value)); \
        }                                                               \
    } while (0)

// Base macro used to implement all typed binary assertions (==, !=, <, etc.)
#define BUT_ASSERT_BINOP_TYPED(lhs, rhs, op, op_str, fmt)                                                                  \
    do {                                                                                                                   \
        if (!((lhs)op(rhs))) {                                                                                             \
            BUT_ASSERT_IMPL("Expected: " fmt " " op_str " " fmt ". Actual: " fmt " and " fmt, (lhs), (rhs), (lhs), (rhs)); \
        }                                                                                                                  \
    } while (0)

#define BUT_ASSERT_EQ_INT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, ==, "==", "%d")
#define BUT_ASSERT_NEQ_INT(expected, actual) BUT_ASSERT_BINOP_TYPED(expected, actual, !=, "!=", "%d")
#define BUT_ASSERT_LT_INT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <, "<", "%d")
#define BUT_ASSERT_LE_INT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <=, "<=", "%d")
#define BUT_ASSERT_GT_INT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >, ">", "%d")
#define BUT_ASSERT_GE_INT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >=, ">=", "%d")

#define BUT_ASSERT_EQ_UINT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, ==, "==", "%u")
#define BUT_ASSERT_NEQ_UINT(expected, actual) BUT_ASSERT_BINOP_TYPED(expected, actual, !=, "!=", "%u")
#define BUT_ASSERT_LT_UINT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <, "<", "%u")
#define BUT_ASSERT_LE_UINT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <=, "<=", "%u")
#define BUT_ASSERT_GT_UINT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >, ">", "%u")
#define BUT_ASSERT_GE_UINT(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >=, ">=", "%u")

#define BUT_ASSERT_EQ_LONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, ==, "==", "%ld")
#define BUT_ASSERT_NEQ_LONG(expected, actual) BUT_ASSERT_BINOP_TYPED(expected, actual, !=, "!=", "%ld")
#define BUT_ASSERT_LT_LONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <, "<", "%ld")
#define BUT_ASSERT_LE_LONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <=, "<=", "%ld")
#define BUT_ASSERT_GT_LONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >, ">", "%ld")
#define BUT_ASSERT_GE_LONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >=, ">=", "%ld")

#define BUT_ASSERT_EQ_ULONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, ==, "==", "%lu")
#define BUT_ASSERT_NEQ_ULONG(expected, actual) BUT_ASSERT_BINOP_TYPED(expected, actual, !=, "!=", "%lu")
#define BUT_ASSERT_LT_ULONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <, "<", "%lu")
#define BUT_ASSERT_LE_ULONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <=, "<=", "%lu")
#define BUT_ASSERT_GT_ULONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >, ">", "%lu")
#define BUT_ASSERT_GE_ULONG(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >=, ">=", "%lu")

#define BUT_ASSERT_EQ_SIZE_T(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, ==, "==", "%zu")
#define BUT_ASSERT_NEQ_SIZE_T(expected, actual) BUT_ASSERT_BINOP_TYPED(expected, actual, !=, "!=", "%zu")
#define BUT_ASSERT_LT_SIZE_T(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <, "<", "%zu")
#define BUT_ASSERT_LE_SIZE_T(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <=, "<=", "%zu")
#define BUT_ASSERT_GT_SIZE_T(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >, ">", "%zu")
#define BUT_ASSERT_GE_SIZE_T(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >=, ">=", "%zu")

#define BUT_ASSERT_EQ_PTRDIFF(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, ==, "==", "%td")
#define BUT_ASSERT_NEQ_PTRDIFF(expected, actual) BUT_ASSERT_BINOP_TYPED(expected, actual, !=, "!=", "%td")
#define BUT_ASSERT_LT_PTRDIFF(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <, "<", "%td")
#define BUT_ASSERT_LE_PTRDIFF(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <=, "<=", "%td")
#define BUT_ASSERT_GT_PTRDIFF(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >, ">", "%td")
#define BUT_ASSERT_GE_PTRDIFF(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >=, ">=", "%td")

#define BUT_ASSERT_EQ_UINT32(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, ==, "==", "%" PRIu32)
#define BUT_ASSERT_NEQ_UINT32(expected, actual) BUT_ASSERT_BINOP_TYPED(expected, actual, !=, "!=", "%" PRIu32)
#define BUT_ASSERT_LT_UINT32(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <, "<", "%" PRIu32)
#define BUT_ASSERT_LE_UINT32(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <=, "<=", "%" PRIu32)
#define BUT_ASSERT_GT_UINT32(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >, ">", "%" PRIu32)
#define BUT_ASSERT_GE_UINT32(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >=, ">=", "%" PRIu32)

#define BUT_ASSERT_EQ_INT64(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, ==, "==", "%" PRId64)
#define BUT_ASSERT_NEQ_INT64(expected, actual) BUT_ASSERT_BINOP_TYPED(expected, actual, !=, "!=", "%" PRId64)
#define BUT_ASSERT_LT_INT64(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <, "<", "%" PRId64)
#define BUT_ASSERT_LE_INT64(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <=, "<=", "%" PRId64)
#define BUT_ASSERT_GT_INT64(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >, ">", "%" PRId64)
#define BUT_ASSERT_GE_INT64(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >=, ">=", "%" PRId64)

#define BUT_ASSERT_EQ_UINTPTR(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, ==, "==", "%" PRIuPTR)
#define BUT_ASSERT_NEQ_UINTPTR(expected, actual) BUT_ASSERT_BINOP_TYPED(expected, actual, !=, "!=", "%" PRIuPTR)
#define BUT_ASSERT_LT_UINTPTR(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <, "<", "%" PRIuPTR)
#define BUT_ASSERT_LE_UINTPTR(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, <=, "<=", "%" PRIuPTR)
#define BUT_ASSERT_GT_UINTPTR(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >, ">", "%" PRIuPTR)
#define BUT_ASSERT_GE_UINTPTR(expected, actual)  BUT_ASSERT_BINOP_TYPED(expected, actual, >=, ">=", "%" PRIuPTR)

// Pointer checks
#define BUT_ASSERT_NULL(ptr)                                      \
    do {                                                          \
        if ((ptr) != NULL) {                                      \
            BUT_ASSERT_IMPL("Expected: NULL. Actual: %p", (ptr)); \
        }                                                         \
    } while (0)

#define BUT_ASSERT_NOT_NULL(ptr)                                              \
    do {                                                                      \
        if ((ptr) == NULL) {                                                  \
            BUT_ASSERT_IMPL("Expected: non-NULL pointer. Actual: %p", (ptr)); \
        }                                                                     \
    } while (0)

#define BUT_ASSERT_EQ_PTR(expected, actual)                                                    \
    do {                                                                                       \
        if ((expected) != (actual)) {                                                          \
            BUT_ASSERT_IMPL("Expected: pointer %p. Actual: pointer %p", (expected), (actual)); \
        }                                                                                      \
    } while (0)

#define BUT_ASSERT_NEQ_PTR(expected, actual)                                            \
    do {                                                                                \
        if ((expected) == (actual)) {                                                   \
            BUT_ASSERT_IMPL("Expected: different pointers. Actual: both %p", (actual)); \
        }                                                                               \
    } while (0)

// String comparisons
#define BUT_ASSERT_STREQ(expected, actual)                                                               \
    do {                                                                                                 \
        const char *_e = (expected);                                                                     \
        const char *_a = (actual);                                                                       \
        if (!_e || !_a || strcmp(_e, _a) != 0) {                                                         \
            BUT_ASSERT_IMPL("Expected: \"%s\". Actual: \"%s\"", _e ? _e : "(null)", _a ? _a : "(null)"); \
        }                                                                                                \
    } while (0)

#define BUT_ASSERT_STRNEQ(expected, actual)                                                     \
    do {                                                                                        \
        const char *_e = (expected);                                                            \
        const char *_a = (actual);                                                              \
        if (!_e || !_a || strcmp(_e, _a) == 0) {                                                \
            BUT_ASSERT_IMPL("Expected: different strings. Actual: \"%s\"", _a ? _a : "(null)"); \
        }                                                                                       \
    } while (0)

// Memory comparison
#define BUT_ASSERT_MEM_EQ(expected, actual, size)                                               \
    do {                                                                                        \
        if (memcmp((expected), (actual), (size)) != 0) {                                        \
            BUT_ASSERT_IMPL("Expected: identical memory blocks for %zu bytes", (size_t)(size)); \
        }                                                                                       \
    } while (0)

#define BUT_ASSERT_MEM_NEQ(expected, actual, size)                                              \
    do {                                                                                        \
        if (memcmp((expected), (actual), (size)) == 0) {                                        \
            BUT_ASSERT_IMPL("Expected: different memory blocks for %zu bytes", (size_t)(size)); \
        }                                                                                       \
    } while (0)

#define BUT_ASSERT_FLOAT_EQ(expected, actual, epsilon)                                               \
    do {                                                                                             \
        if (fabs((expected) - (actual)) > (epsilon)) {                                               \
            BUT_ASSERT_IMPL("Expected: %.6f ± %.6f. Actual: %.6f", (expected), (epsilon), (actual)); \
        }                                                                                            \
    } while (0)

#define BUT_ASSERT_DOUBLE_EQ(expected, actual, epsilon)                                                 \
    do {                                                                                                \
        if (fabs((expected) - (actual)) > (epsilon)) {                                                  \
            BUT_ASSERT_IMPL("Expected: %.6lf ± %.6lf. Actual: %.6lf", (expected), (epsilon), (actual)); \
        }                                                                                               \
    } while (0)

// Generic macros (type-agnostic), but requires C11 or later for _Generic support
// Customize the fallback behavior of the generic assertions. Override with:
//  #define BUT_ASSERT_EQ_CUSTOM MyCustomType: MY_ASSERT_EQ

// clang format adds extra lines to these macros, so we disable it for this section
// clang-format off
#ifndef BUT_ASSERT_EQ_CUSTOM
#define BUT_ASSERT_EQ_CUSTOM default: BUT_ASSERT_EQ_INT
#endif

#ifndef BUT_ASSERT_NEQ_CUSTOM
#define BUT_ASSERT_NEQ_CUSTOM default: BUT_ASSERT_NEQ_INT
#endif

#ifndef BUT_ASSERT_LT_CUSTOM
#define BUT_ASSERT_LT_CUSTOM default: BUT_ASSERT_LT_INT
#endif

#ifndef BUT_ASSERT_LE_CUSTOM
#define BUT_ASSERT_LE_CUSTOM default: BUT_ASSERT_LE_INT
#endif

#ifndef BUT_ASSERT_GT_CUSTOM
#define BUT_ASSERT_GT_CUSTOM default: BUT_ASSERT_GT_INT
#endif

#ifndef BUT_ASSERT_GE_CUSTOM
#define BUT_ASSERT_GE_CUSTOM default: BUT_ASSERT_GE_INT
#endif
// clang-format on

#define BUT_ASSERT_EQ(expected, actual)     \
    _Generic((expected),                    \
        int: BUT_ASSERT_EQ_INT,             \
        unsigned int: BUT_ASSERT_EQ_UINT,   \
        long: BUT_ASSERT_EQ_LONG,           \
        unsigned long: BUT_ASSERT_EQ_ULONG, \
        size_t: BUT_ASSERT_EQ_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_EQ_PTRDIFF,   \
        uint32_t: BUT_ASSERT_EQ_UINT32,     \
        int64_t: BUT_ASSERT_EQ_INT64,       \
        uintptr_t: BUT_ASSERT_EQ_UINTPTR,   \
        void *: BUT_ASSERT_EQ_PTR,          \
        char *: BUT_ASSERT_STREQ,           \
        const char *: BUT_ASSERT_STREQ,     \
        BUT_ASSERT_EQ_CUSTOM)(expected, actual)

#define BUT_ASSERT_NEQ(expected, actual)     \
    _Generic((expected),                     \
        int: BUT_ASSERT_NEQ_INT,             \
        unsigned int: BUT_ASSERT_NEQ_UINT,   \
        long: BUT_ASSERT_NEQ_LONG,           \
        unsigned long: BUT_ASSERT_NEQ_ULONG, \
        size_t: BUT_ASSERT_NEQ_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_NEQ_PTRDIFF,   \
        uint32_t: BUT_ASSERT_NEQ_UINT32,     \
        int64_t: BUT_ASSERT_NEQ_INT64,       \
        uintptr_t: BUT_ASSERT_NEQ_UINTPTR,   \
        void *: BUT_ASSERT_NEQ_PTR,          \
        char *: BUT_ASSERT_STRNEQ,           \
        const char *: BUT_ASSERT_STRNEQ,     \
        BUT_ASSERT_NEQ_CUSTOM)(expected, actual)

#define BUT_ASSERT_LT(expected, actual)     \
    _Generic((expected),                    \
        int: BUT_ASSERT_LT_INT,             \
        unsigned int: BUT_ASSERT_LT_UINT,   \
        long: BUT_ASSERT_LT_LONG,           \
        unsigned long: BUT_ASSERT_LT_ULONG, \
        size_t: BUT_ASSERT_LT_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_LT_PTRDIFF,   \
        uint32_t: BUT_ASSERT_LT_UINT32,     \
        int64_t: BUT_ASSERT_LT_INT64,       \
        uintptr_t: BUT_ASSERT_LT_UINTPTR,   \
        BUT_ASSERT_LT_CUSTOM)(expected, actual)

#define BUT_ASSERT_LE(expected, actual)     \
    _Generic((expected),                    \
        int: BUT_ASSERT_LE_INT,             \
        unsigned int: BUT_ASSERT_LE_UINT,   \
        long: BUT_ASSERT_LE_LONG,           \
        unsigned long: BUT_ASSERT_LE_ULONG, \
        size_t: BUT_ASSERT_LE_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_LE_PTRDIFF,   \
        uint32_t: BUT_ASSERT_LE_UINT32,     \
        int64_t: BUT_ASSERT_LE_INT64,       \
        uintptr_t: BUT_ASSERT_LE_UINTPTR,   \
        BUT_ASSERT_LE_CUSTOM)(expected, actual)

#define BUT_ASSERT_GT(expected, actual)     \
    _Generic((expected),                    \
        int: BUT_ASSERT_GT_INT,             \
        unsigned int: BUT_ASSERT_GT_UINT,   \
        long: BUT_ASSERT_GT_LONG,           \
        unsigned long: BUT_ASSERT_GT_ULONG, \
        size_t: BUT_ASSERT_GT_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_GT_PTRDIFF,   \
        uint32_t: BUT_ASSERT_GT_UINT32,     \
        int64_t: BUT_ASSERT_GT_INT64,       \
        uintptr_t: BUT_ASSERT_GT_UINTPTR,   \
        BUT_ASSERT_GT_CUSTOM)(expected, actual)

#define BUT_ASSERT_GE(expected, actual)     \
    _Generic((expected),                    \
        int: BUT_ASSERT_GE_INT,             \
        unsigned int: BUT_ASSERT_GE_UINT,   \
        long: BUT_ASSERT_GE_LONG,           \
        unsigned long: BUT_ASSERT_GE_ULONG, \
        size_t: BUT_ASSERT_GE_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_GE_PTRDIFF,   \
        uint32_t: BUT_ASSERT_GE_UINT32,     \
        int64_t: BUT_ASSERT_GE_INT64,       \
        uintptr_t: BUT_ASSERT_GE_UINTPTR,   \
        BUT_ASSERT_GE_CUSTOM)(expected, actual)

#if defined(__cplusplus)
}
#endif

#endif // BUT_ASSERT_H_
