#ifndef BUT_EXCEPTION_ASSERT_H_
#define BUT_EXCEPTION_ASSERT_H_

/**
 * @file assert.h
 * @author Douglas Cuthbertson
 * @brief Assertions based on an exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */

#if defined(__cplusplus)
extern "C" {
#endif

// define our own assert macro
#ifdef assert
#undef assert
#endif

#if !defined(_DEBUG)
#include <but_macros.h> // BUT_UNUSED

#define assert(e)      \
    do {               \
        BUT_UNUSED(e); \
    } while (0)

#define BUT_ASSERT(e)  \
    do {               \
        BUT_UNUSED(e); \
    } while (0)

#define BUT_ASSERT_FILE_LINE(e, FILE, LINE) \
    do {                                    \
        BUT_UNUSED(e);                      \
        BUT_UNUSED(FILE);                   \
        BUT_UNUSED(LINE);                   \
    } while (0)
#else
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Throw an exception when an assertion fails. Include the file name and
 * line number on which the assertion failed.
 *
 * @param reason a string representing the assertion failure.
 * @param file_name the full path to the file in which the assertion failed.
 * @param line the line number of the failed assertion.
 */
void but_throw_assertion(char const *reason, char const *file_name, int line);

/**
 * @brief test an assertion; if it fails throw an exception where the reason is
 * a string representing the assertion.
 */
#define BUT_ASSERT(e)                                      \
    do {                                                   \
        if (!(e)) {                                        \
            but_throw_assertion((#e), __FILE__, __LINE__); \
        }                                                  \
    } while (0)

/**
 * @brief test an assertion; if it fails throw an exception where the reason is
 * a string representing the assertion.
 */
#define BUT_ASSERT_FILE_LINE(e, FILE, LINE)        \
    do {                                           \
        if (!(e)) {                                \
            but_throw_assertion((#e), FILE, LINE); \
        }                                          \
    } while (0)

/**
 * @brief redefine assert(e) to use the BUT_ASSERT(e) macro.
 *
 */
#define assert(e) BUT_ASSERT(e)

#if defined(__cplusplus)
}
#endif
#endif // _DEBUG

#if defined(__cplusplus)
}
#endif

#endif // BUT_EXCEPTION_ASSERT_H_
