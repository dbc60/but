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
#define BUT_ASSERT_IMPL(msg, ...) \
    BUT_THROW_DETAILS(but_unexpected_failure, msg, ##__VA_ARGS__)

// Helper macro that accepts a reason string so failed assertions can have a reason other
// than but_unexpected_failure
#define BUT_ASSERT_REASON_IMPL(REASON, msg, ...) \
    BUT_THROW_DETAILS(REASON, msg, ##__VA_ARGS__)

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
#define BUT_ASSERT_ZERO(value)                                                      \
    do {                                                                            \
        if ((value) != 0) {                                                         \
            BUT_ASSERT_REASON_IMPL(but_invalid_value, "Expected: zero. Actual: %d", \
                                   (value));                                        \
        }                                                                           \
    } while (0)

#define BUT_ASSERT_NON_ZERO(value)                                                      \
    do {                                                                                \
        if ((value) == 0) {                                                             \
            BUT_ASSERT_REASON_IMPL(but_invalid_value, "Expected: non-zero. Actual: %d", \
                                   (value));                                            \
        }                                                                               \
    } while (0)

// Base macro used to implement all typed binary assertions (==, !=, <, etc.)
#define BUT_ASSERT_BINOP_TYPED(lhs, rhs, op, op_str, fmt)                        \
    do {                                                                         \
        if (!((lhs)op(rhs))) {                                                   \
            BUT_ASSERT_IMPL("Expected: " fmt " " op_str " " fmt ". Actual: " fmt \
                            " and " fmt,                                         \
                            (lhs), (rhs), (lhs), (rhs));                         \
        }                                                                        \
    } while (0)

#define BUT_ASSERT_EQ_INT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, ==, "==", "%d")
#define BUT_ASSERT_NEQ_INT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, !=, "!=", "%d")
#define BUT_ASSERT_LT_INT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <, "<", "%d")
#define BUT_ASSERT_LE_INT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <=, "<=", "%d")
#define BUT_ASSERT_GT_INT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >, ">", "%d")
#define BUT_ASSERT_GE_INT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >=, ">=", "%d")

#define BUT_ASSERT_EQ_UINT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, ==, "==", "%u")
#define BUT_ASSERT_NEQ_UINT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, !=, "!=", "%u")
#define BUT_ASSERT_LT_UINT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <, "<", "%u")
#define BUT_ASSERT_LE_UINT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <=, "<=", "%u")
#define BUT_ASSERT_GT_UINT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >, ">", "%u")
#define BUT_ASSERT_GE_UINT(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >=, ">=", "%u")

#define BUT_ASSERT_EQ_LONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, ==, "==", "%ld")
#define BUT_ASSERT_NEQ_LONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, !=, "!=", "%ld")
#define BUT_ASSERT_LT_LONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <, "<", "%ld")
#define BUT_ASSERT_LE_LONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <=, "<=", "%ld")
#define BUT_ASSERT_GT_LONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >, ">", "%ld")
#define BUT_ASSERT_GE_LONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >=, ">=", "%ld")

#define BUT_ASSERT_EQ_ULONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, ==, "==", "%lu")
#define BUT_ASSERT_NEQ_ULONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, !=, "!=", "%lu")
#define BUT_ASSERT_LT_ULONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <, "<", "%lu")
#define BUT_ASSERT_LE_ULONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <=, "<=", "%lu")
#define BUT_ASSERT_GT_ULONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >, ">", "%lu")
#define BUT_ASSERT_GE_ULONG(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >=, ">=", "%lu")

#define BUT_ASSERT_EQ_SIZE_T(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, ==, "==", "%zu")
#define BUT_ASSERT_NEQ_SIZE_T(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, !=, "!=", "%zu")
#define BUT_ASSERT_LT_SIZE_T(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <, "<", "%zu")
#define BUT_ASSERT_LE_SIZE_T(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <=, "<=", "%zu")
#define BUT_ASSERT_GT_SIZE_T(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >, ">", "%zu")
#define BUT_ASSERT_GE_SIZE_T(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >=, ">=", "%zu")

#define BUT_ASSERT_EQ_PTRDIFF(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, ==, "==", "%td")
#define BUT_ASSERT_NEQ_PTRDIFF(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, !=, "!=", "%td")
#define BUT_ASSERT_LT_PTRDIFF(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <, "<", "%td")
#define BUT_ASSERT_LE_PTRDIFF(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <=, "<=", "%td")
#define BUT_ASSERT_GT_PTRDIFF(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >, ">", "%td")
#define BUT_ASSERT_GE_PTRDIFF(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >=, ">=", "%td")

#define BUT_ASSERT_EQ_UINT32(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, ==, "==", "%" PRIu32)
#define BUT_ASSERT_NEQ_UINT32(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, !=, "!=", "%" PRIu32)
#define BUT_ASSERT_LT_UINT32(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <, "<", "%" PRIu32)
#define BUT_ASSERT_LE_UINT32(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <=, "<=", "%" PRIu32)
#define BUT_ASSERT_GT_UINT32(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >, ">", "%" PRIu32)
#define BUT_ASSERT_GE_UINT32(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >=, ">=", "%" PRIu32)

#define BUT_ASSERT_EQ_INT64(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, ==, "==", "%" PRId64)
#define BUT_ASSERT_NEQ_INT64(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, !=, "!=", "%" PRId64)
#define BUT_ASSERT_LT_INT64(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <, "<", "%" PRId64)
#define BUT_ASSERT_LE_INT64(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <=, "<=", "%" PRId64)
#define BUT_ASSERT_GT_INT64(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >, ">", "%" PRId64)
#define BUT_ASSERT_GE_INT64(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >=, ">=", "%" PRId64)

#define BUT_ASSERT_EQ_UINTPTR(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, ==, "==", "%" PRIuPTR)
#define BUT_ASSERT_NEQ_UINTPTR(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, !=, "!=", "%" PRIuPTR)
#define BUT_ASSERT_LT_UINTPTR(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <, "<", "%" PRIuPTR)
#define BUT_ASSERT_LE_UINTPTR(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, <=, "<=", "%" PRIuPTR)
#define BUT_ASSERT_GT_UINTPTR(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >, ">", "%" PRIuPTR)
#define BUT_ASSERT_GE_UINTPTR(EXPECTED, ACTUAL) \
    BUT_ASSERT_BINOP_TYPED(EXPECTED, ACTUAL, >=, ">=", "%" PRIuPTR)

// Pointer checks
#define BUT_ASSERT_NULL(PTR)                                                        \
    do {                                                                            \
        if ((PTR) != NULL) {                                                        \
            BUT_ASSERT_REASON_IMPL(but_invalid_value, "Expected: NULL. Actual: %p", \
                                   (PTR));                                          \
        }                                                                           \
    } while (0)

#define BUT_ASSERT_NOT_NULL(PTR)                                                     \
    do {                                                                             \
        if ((PTR) == NULL) {                                                         \
            BUT_ASSERT_REASON_IMPL(but_invalid_value,                                \
                                   "Expected: non-NULL pointer. Actual: %p", (PTR)); \
        }                                                                            \
    } while (0)

#define BUT_ASSERT_EQ_PTR(EXPECTED, ACTUAL)                                         \
    do {                                                                            \
        if ((EXPECTED) != (ACTUAL)) {                                               \
            BUT_ASSERT_IMPL("Expected: pointer %p. Actual: pointer %p", (EXPECTED), \
                            (ACTUAL));                                              \
        }                                                                           \
    } while (0)

#define BUT_ASSERT_NEQ_PTR(EXPECTED, ACTUAL)                                            \
    do {                                                                                \
        if ((EXPECTED) == (ACTUAL)) {                                                   \
            BUT_ASSERT_IMPL("Expected: different pointers. Actual: both %p", (ACTUAL)); \
        }                                                                               \
    } while (0)

// String comparisons
#define BUT_ASSERT_STREQ(EXPECTED, ACTUAL)                                          \
    do {                                                                            \
        const char *_e = (EXPECTED);                                                \
        const char *_a = (ACTUAL);                                                  \
        if (!_e || !_a || strcmp(_e, _a) != 0) {                                    \
            BUT_ASSERT_IMPL("Expected: \"%s\". Actual: \"%s\"", _e ? _e : "(null)", \
                            _a ? _a : "(null)");                                    \
        }                                                                           \
    } while (0)

#define BUT_ASSERT_STRNEQ(EXPECTED, ACTUAL)                                \
    do {                                                                   \
        const char *_e = (EXPECTED);                                       \
        const char *_a = (ACTUAL);                                         \
        if (!_e || !_a || strcmp(_e, _a) == 0) {                           \
            BUT_ASSERT_IMPL("Expected: different strings. Actual: \"%s\"", \
                            _a ? _a : "(null)");                           \
        }                                                                  \
    } while (0)

// Memory comparison
#define BUT_ASSERT_MEM_EQ(EXPECTED, ACTUAL, size)                              \
    do {                                                                       \
        if (memcmp((EXPECTED), (ACTUAL), (size)) != 0) {                       \
            BUT_ASSERT_IMPL("Expected: identical memory blocks for %zu bytes", \
                            (size_t)(size));                                   \
        }                                                                      \
    } while (0)

#define BUT_ASSERT_MEM_NEQ(EXPECTED, ACTUAL, size)                             \
    do {                                                                       \
        if (memcmp((EXPECTED), (ACTUAL), (size)) == 0) {                       \
            BUT_ASSERT_IMPL("Expected: different memory blocks for %zu bytes", \
                            (size_t)(size));                                   \
        }                                                                      \
    } while (0)

#define BUT_ASSERT_FLOAT_EQ(EXPECTED, ACTUAL, epsilon)                         \
    do {                                                                       \
        if (fabs((EXPECTED) - (ACTUAL)) > (epsilon)) {                         \
            BUT_ASSERT_IMPL("Expected: %.6f ± %.6f. Actual: %.6f", (EXPECTED), \
                            (epsilon), (ACTUAL));                              \
        }                                                                      \
    } while (0)

#define BUT_ASSERT_DOUBLE_EQ(EXPECTED, ACTUAL, epsilon)                           \
    do {                                                                          \
        if (fabs((EXPECTED) - (ACTUAL)) > (epsilon)) {                            \
            BUT_ASSERT_IMPL("Expected: %.6lf ± %.6lf. Actual: %.6lf", (EXPECTED), \
                            (epsilon), (ACTUAL));                                 \
        }                                                                         \
    } while (0)

#define BUT_ASSERT_SIZE_RANGE(size, min, max, file, line)                               \
    do {                                                                                \
        if ((size) < (min) || (size) > (max)) {                                         \
            BUT_THROW_DETAILS_FILE_LINE(but_invalid_value,                              \
                                        "size %zu out of range [%zu, %zu]", file, line, \
                                        (size_t)(size), (size_t)(min), (size_t)(max));  \
        }                                                                               \
    } while (0)

#define BUT_ASSERT_MULTIPLICATION_OVERFLOW(a, b, file, line)                        \
    do {                                                                            \
        if ((a) > 0 && (b) > SIZE_MAX / (a)) {                                      \
            BUT_THROW_DETAILS_FILE_LINE(but_invalid_value,                          \
                                        "multiplication overflow: %zu * %zu", file, \
                                        line, (size_t)(a), (size_t)(b));            \
        }                                                                           \
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

#define BUT_ASSERT_EQ(EXPECTED, ACTUAL)     \
    _Generic((EXPECTED),                    \
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
        BUT_ASSERT_EQ_CUSTOM)(EXPECTED, ACTUAL)

#define BUT_ASSERT_NEQ(EXPECTED, ACTUAL)     \
    _Generic((EXPECTED),                     \
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
        BUT_ASSERT_NEQ_CUSTOM)(EXPECTED, ACTUAL)

#define BUT_ASSERT_LT(EXPECTED, ACTUAL)     \
    _Generic((EXPECTED),                    \
        int: BUT_ASSERT_LT_INT,             \
        unsigned int: BUT_ASSERT_LT_UINT,   \
        long: BUT_ASSERT_LT_LONG,           \
        unsigned long: BUT_ASSERT_LT_ULONG, \
        size_t: BUT_ASSERT_LT_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_LT_PTRDIFF,   \
        uint32_t: BUT_ASSERT_LT_UINT32,     \
        int64_t: BUT_ASSERT_LT_INT64,       \
        uintptr_t: BUT_ASSERT_LT_UINTPTR,   \
        BUT_ASSERT_LT_CUSTOM)(EXPECTED, ACTUAL)

#define BUT_ASSERT_LE(EXPECTED, ACTUAL)     \
    _Generic((EXPECTED),                    \
        int: BUT_ASSERT_LE_INT,             \
        unsigned int: BUT_ASSERT_LE_UINT,   \
        long: BUT_ASSERT_LE_LONG,           \
        unsigned long: BUT_ASSERT_LE_ULONG, \
        size_t: BUT_ASSERT_LE_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_LE_PTRDIFF,   \
        uint32_t: BUT_ASSERT_LE_UINT32,     \
        int64_t: BUT_ASSERT_LE_INT64,       \
        uintptr_t: BUT_ASSERT_LE_UINTPTR,   \
        BUT_ASSERT_LE_CUSTOM)(EXPECTED, ACTUAL)

#define BUT_ASSERT_GT(EXPECTED, ACTUAL)     \
    _Generic((EXPECTED),                    \
        int: BUT_ASSERT_GT_INT,             \
        unsigned int: BUT_ASSERT_GT_UINT,   \
        long: BUT_ASSERT_GT_LONG,           \
        unsigned long: BUT_ASSERT_GT_ULONG, \
        size_t: BUT_ASSERT_GT_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_GT_PTRDIFF,   \
        uint32_t: BUT_ASSERT_GT_UINT32,     \
        int64_t: BUT_ASSERT_GT_INT64,       \
        uintptr_t: BUT_ASSERT_GT_UINTPTR,   \
        BUT_ASSERT_GT_CUSTOM)(EXPECTED, ACTUAL)

#define BUT_ASSERT_GE(EXPECTED, ACTUAL)     \
    _Generic((EXPECTED),                    \
        int: BUT_ASSERT_GE_INT,             \
        unsigned int: BUT_ASSERT_GE_UINT,   \
        long: BUT_ASSERT_GE_LONG,           \
        unsigned long: BUT_ASSERT_GE_ULONG, \
        size_t: BUT_ASSERT_GE_SIZE_T,       \
        ptrdiff_t: BUT_ASSERT_GE_PTRDIFF,   \
        uint32_t: BUT_ASSERT_GE_UINT32,     \
        int64_t: BUT_ASSERT_GE_INT64,       \
        uintptr_t: BUT_ASSERT_GE_UINTPTR,   \
        BUT_ASSERT_GE_CUSTOM)(EXPECTED, ACTUAL)

#if defined(__cplusplus)
}
#endif

#endif // BUT_ASSERT_H_
