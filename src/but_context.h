#ifndef BUT_CONTEXT_H_
#define BUT_CONTEXT_H_

/**
 * @file but_context.h
 * @author Douglas Cuthbertson
 * @brief
 * @version 0.1
 * @date 2025-02-09
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 *
 */
#include <but.h>               // BUTTestCase and BUTTestSuite
#include <exception_types.h>   // BUTExceptionContext
#include <abbreviated_types.h> // u32

#include <stdbool.h> // bool
#include <stdint.h>  // uintptr_t

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum BUTResultCode {
    BUT_PASSED,         ///< The test case ran and it returned successfully
    BUT_FAILED,         ///< The test case ran and it threw an exception
    BUT_FAILED_SETUP,   ///< The setup function threw an exception
    BUT_FAILED_CLEANUP, ///< the cleanup function threw an exception
    BUT_NOT_RUN         ///< The test case has not run
} BUTResultCode;

typedef struct ResultContext ResultContext;

/**
 * @brief A BUTEnvironment is used to iterate through the test cases in a test suite,
 * keep track of the tests that have been exercised, which tests remain, and the results
 * of each test.
 */
typedef struct BUTEnvironment {
    bool                 initialized;      ///< indicates a valid context
    struct BUTTestSuite *bts;              ///< the test suite under test
    u32                  test_case_count;  ///< the number of test cases in the suite
    u32                  index;            ///< index of the current test case
    u32                  run_count;        ///< number of tests run
    u32                  test_failures;    ///< number of tests that ran and failed
    u32                  setup_failures;   ///< number of tests that failed setup
    u32                  cleanup_failures; ///< tests that failed cleanup
    u32                  results_count;    ///< number of test results
    u32                  results_capacity; ///< number of results that can be stored
    ResultContext       *results;          ///< a resizable array of test results.
} BUTEnvironment;

/**
 * @brief A test context combines an exception context and a test environment.
 */
typedef struct BUTContext {
    BUTExceptionContext but_ctx;
    BUTEnvironment      env;
} BUTContext;

#if defined(__cplusplus)
}
#endif

#endif // BUT_CONTEXT_H_
