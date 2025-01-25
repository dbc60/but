#ifndef CRY_EXCEPTIONS_ASSERT_H_
#define CRY_EXCEPTIONS_ASSERT_H_

/**
 * @file assert.h
 * @author Douglas Cuthbertson
 * @brief Assertions based on an exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
// define our own assert macro
#ifdef assert
#undef assert
#endif

#if !defined(DEBUG)
#ifdef NDEBUG
#undef NDEBUG
#endif
#endif

#ifdef NDEBUG
#define assert(e) ((void)0)
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
void exm_throw_assertion(char const *reason, char const *file_name, int line);

/**
 * @brief test an assertion; if it fails throw an exception where the reason is
 * a string representing the assertion.
 */
#define EXM_ASSERT(e)                                      \
    do {                                                   \
        if (!(e)) {                                        \
            exm_throw_assertion((#e), __FILE__, __LINE__); \
        }                                                  \
    } while (0)

#define EXM_ASSERT_FILE_LINE(e, FILE, LINE)        \
    do {                                           \
        if (!(e)) {                                \
            exm_throw_assertion((#e), FILE, LINE); \
        }                                          \
    } while (0)

/**
 * @brief a function wrapper around the assert macro.
 *
 * @param e the condition being asserted.
 */
extern void assert(int e);

/**
 * @brief redefine assert(e) to use the EXM_ASSERT(e) macro.
 *
 */
#define assert(e) EXM_ASSERT(e)

#if defined(__cplusplus)
}
#endif
#endif // NDEBUG
#endif // CRY_EXCEPTIONS_ASSERT_H_
