#ifndef BUT_H_
#define BUT_H_

/**
 * @file but.h
 * @author Douglas Cuthbertson
 * @brief definitions for Basic Unit Test (BUT) test cases and test suites.
 * @version 0.2
 * @date 2005-10-14
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <exception.h>         // but_expected_failure
#include <abbreviated_types.h> // u32
#include <but_macros.h>        // BUT_UNUSED

#include <string.h> // strcmp

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief produce a non-zero value for any exception except but_expected_failure.
 *
 * We use strcmp(), because the exception library is statically linked to both
 * executables (like a test driver) and shared libraries (like a test suite).
 * That means the address of an exception thrown by a shared library and caught
 * by a test driver won't be the same, but their string-values will be.
 */
#define BUT_UNEXPECTED_EXCEPTION(e) strcmp((e), but_expected_failure)

// Forward declaration to suppress "warning C4115: 'BUTTestCase': named type
// definition in parentheses"
struct BUTTestCase;
struct BUTTestSuite;

//////////////////////////////////////////////////////////////////
/////////////////// DEFINE TEST FUNCTIONS ////////////////////////
//////////////////////////////////////////////////////////////////

// Setup is intended to initialize a test case by acquiring any necessary
// resources. If it fails to acquire those resources, it should throw an
// exception that includes the reason for the failure.
#define BUT_SETUP_FN(NAME) void NAME(struct BUTTestCase *btc)
typedef BUT_SETUP_FN(but_setup_fn);

// Run is the function that runs a test case. It should throw an
// exception on failure and the exception should state the reason for the
// failure.
#define BUT_TEST_FN(NAME) void NAME(struct BUTTestCase *btc)
typedef BUT_TEST_FN(but_test_fn);

// Cleanup is intended to release any resources acquired by but_setup.
#define BUT_CLEANUP_FN(NAME) void NAME(struct BUTTestCase *btc)
typedef BUT_CLEANUP_FN(but_cleanup_fn);

//////////////////////////////////////////////////////////////////
////////////////// MACROS TO DEFINE TEST CASES ///////////////////
//////////////////////////////////////////////////////////////////
#define BUT_CASE(NAME, TEST, SETUP, CLEANUP) \
    static BUTTestCase TEST##_case = {       \
        .name    = NAME,                     \
        .setup   = SETUP,                    \
        .test    = TEST,                     \
        .cleanup = CLEANUP,                  \
    }

#define BUT_CASE_NAME(NAME, CASE_NAME, TEST, SETUP, CLEANUP) \
    static BUTTestCase CASE_NAME##_case = {                  \
        .name    = NAME,                                     \
        .setup   = SETUP,                                    \
        .test    = TEST,                                     \
        .cleanup = CLEANUP,                                  \
    }

/**
 * @brief Define a test case with a name and a test function.
 *
 * @param NAME The name of the test case as a string.
 * @param TEST The test function to run.
 */
#define BUT_TEST(NAME, TEST)                              \
    static void TEST(void);                               \
    static void TEST##_wrapper(struct BUTTestCase *btc) { \
        BUT_UNUSED(btc);                                  \
        TEST();                                           \
    }                                                     \
    static BUTTestCase TEST##_case = {                    \
        .name    = NAME,                                  \
        .setup   = NULL,                                  \
        .test    = TEST##_wrapper,                        \
        .cleanup = NULL,                                  \
    };                                                    \
    static void TEST(void)

/**
 * @brief Define a test case with setup and cleanup functions.
 *
 * @param NAME The name of the test case as a string.
 * @param TEST The test function to run.
 * @param SETUP The setup function to run before the test.
 * @param CLEANUP The cleanup function to run after the test.
 */
#define BUT_TEST_SETUP_CLEANUP(NAME, TEST, SETUP, CLEANUP) \
    static void TEST(void);                                \
    static void TEST##_wrapper(struct BUTTestCase *btc) {  \
        BUT_UNUSED(btc);                                   \
        TEST();                                            \
    }                                                      \
    static BUTTestCase TEST##_case = {                     \
        .name    = (NAME),                                 \
        .setup   = (SETUP),                                \
        .test    = TEST##_wrapper,                         \
        .cleanup = (CLEANUP),                              \
    };                                                     \
    static void TEST(void)

/**
 * @brief Macro to mark the test-case parameter in the test function as unused.
 */
#define BUT_UNUSED_TYPE_ARG BUT_UNUSED(t)

/**
 * @brief Define a test case derived from BUTTestCase with setup and cleanup functions.
 *
 * @param NAME The name of the test case as a string.
 * @param TYPE The type of the test case.
 * @param TEST The test function to run.
 * @param SETUP The setup function to run before the test.
 * @param CLEANUP The cleanup function to run after the test.
 */
#define BUT_TYPE_TEST_SETUP_CLEANUP(NAME, TYPE, TEST, SETUP, CLEANUP) \
    static void TEST(TYPE *t);                                        \
    static void TEST##_wrapper(struct BUTTestCase *btc) {             \
        TYPE *t = BUT_CONTAINER(btc, TYPE, btc);                      \
        TEST(t);                                                      \
    }                                                                 \
    TYPE TEST##_case = {                                              \
        .btc.name    = NAME,                                          \
        .btc.setup   = SETUP,                                         \
        .btc.test    = TEST##_wrapper,                                \
        .btc.cleanup = CLEANUP,                                       \
    };                                                                \
    static void TEST(TYPE *t)

/**
 * @brief Sometimes setup and cleanup deal with side effects that the test doesn't use
 * directly, so this macro defines a test that takes no arguments, while allowing setup
 * and cleanup to accept a pointer to a struct with an embedded BUTTestCase.
 *
 * @param NAME The name of the test case
 * @param TYPE a type (struct) with an embedded BUTTestCase
 * @param TEST The test function to run.
 * @param SETUP The setup function to run before the test.
 * @param CLEANUP The cleanup function to run after the test.
 */
#define BUT_VOID_TEST_SETUP_CLEANUP(NAME, TYPE, TEST, SETUP, CLEANUP) \
    static void TEST(void);                                           \
    static void TEST##_wrapper(struct BUTTestCase *btc) {             \
        BUT_UNUSED(btc);                                              \
        TEST();                                                       \
    }                                                                 \
    TYPE TEST##_case = {                                              \
        .btc.name    = NAME,                                          \
        .btc.setup   = SETUP,                                         \
        .btc.test    = TEST##_wrapper,                                \
        .btc.cleanup = CLEANUP,                                       \
    };                                                                \
    static void TEST(void)

// helper macro for defining test suites
#define BUT_PTR(X) (&(X).btc)

#define BUT_TEST_SUITE_NAME(SUITE) SUITE##_ts

#define BUT_TEST_SUITE(NAME, SUITE)                                      \
    static BUTTestSuite SUITE##_ts                                       \
        = {.name       = NAME,                                           \
           .count      = sizeof SUITE##_cases / sizeof SUITE##_cases[0], \
           .test_cases = SUITE##_cases}

// Define suite with auto count
#define BUT_GET_TEST_SUITE(NAME, SUITE)                                  \
    static BUTTestSuite SUITE##_ts                                       \
        = {.name       = NAME,                                           \
           .count      = sizeof SUITE##_cases / sizeof SUITE##_cases[0], \
           .test_cases = SUITE##_cases};                                 \
    DLL_SPEC_EXPORT BUTTestSuite *get_test_suite(void) {                 \
        return &SUITE##_ts;                                              \
    }

// a macro to define a common field for test-case structs to embed a BUTTestCase.
#define BUT_EMBED_CASE BUTTestCase btc

// Auto-register test cases in a suite
#define BUT_SUITE_BEGIN(NAME)      static BUTTestCase *NAME##_cases[] = {
#define BUT_SUITE_ADD(TC)          &TC##_case,
#define BUT_SUITE_ADD_EMBEDDED(TC) &TC##_case.btc,
#define BUT_SUITE_END              }

// A test case has a name, an optional setup function, a test function, and an
// optional cleanup function.
struct BUTTestCase {
    char           *name;
    but_setup_fn   *setup;
    but_test_fn    *test;
    but_cleanup_fn *cleanup;
};
typedef struct BUTTestCase BUTTestCase;

// A test suite has a name and one or more test cases to run.
struct BUTTestSuite {
    char         *name;
    u32           count;
    BUTTestCase **test_cases;
};
typedef struct BUTTestSuite BUTTestSuite;

typedef BUTTestSuite *(*but_get_test_suite)();

#if defined(__cplusplus)
}
#endif

#endif // BUT_H_
