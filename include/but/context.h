#ifndef BUT_CONTEXT_H_
#define BUT_CONTEXT_H_

/**
 * @file context.h
 * @author Douglas Cuthbertson
 * @brief context definitions for the Basic Unit Test (BUT) module.
 * @version 0.1
 * @date 2023-12-09
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <but/but.h>          // BUTTestCase and BUTTestSuite
#include <exceptions/types.h> // EXMClosure
#include <platform.h>         // u32

#include <inttypes.h> // uintptr_t
#include <stdbool.h>  // bool

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
    uintptr_t            magic;            ///< indicates a valid context
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
    EXMClosure     exm_ctx;
    BUTEnvironment env;
} BUTContext;

/**
 * @brief determine if the context has been properly initialized by calls to
 * but_initialize and but_begin.
 *
 * @param ctx a test context
 * @return true if the test context is valid and false otherwise.
 */
bool but_is_valid(BUTContext *ctx);

/**
 * @brief initialize a test context.
 *
 * @param ctx the test context to be initialized.
 */
void but_initialize(BUTContext *ctx);

/**
 * @brief but_begin assigns a test suite to a test context.
 *
 * @param ctx a test context.
 * @param bts a test suite.
 */
void but_begin(BUTContext *ctx, struct BUTTestSuite *bts);

/**
 * @brief but_end releases the memory resources allocated during testing.
 *
 * @param ctx test context
 */
void but_end(BUTContext *ctx);

/**
 * @brief but_next is part of the iterator interface. If there is another test case in
 * the test suite, the context is updated to hold that test case as the current one. If
 * there are no more test cases, the context is unchanged.
 *
 * @param ctx a test context.
 */
void but_next(BUTContext *ctx);

/**
 * @brief but_has_more is part of the iterator interface. It determines if there is
 * another test case in the test suite,
 *
 * @param cts a test context.
 *
 * @return true if there are more test cases after the current one, and false otherwise.
 */
bool but_has_more(BUTContext *ctx);

/**
 * @brief but_get_test_case_name retrieves the name of the current test case.
 *
 * @param ctx a test context.
 * @param bts a test suite.
 *
 * @return the name of the current test case.
 */
const char *but_get_test_case_name(BUTContext *ctx);

/**
 * @brief retrieve the index of the current test case.
 *
 * @param ctx a test context.
 * @return the zero-based index of the test case.
 */
u32 but_get_index(BUTContext *ctx);

/**
 * @brief but_test executes the current test case.
 *
 * @param ctx a test context.
 */
void but_test(BUTContext *ctx);

/**
 * @brief retrieve the number of test cases executed.
 *
 * @param ctx a test context.
 * @return the number of test cases executed.
 */
u32 but_get_run_count(BUTContext *ctx);

/**
 * @brief retrieve the number of tests that passed.
 *
 * @param ctx a test context.
 * @return  the number of tests that passed.
 */
u32 but_get_pass_count(BUTContext *ctx);

/**
 * @brief retrieve the number of tests that failed.
 *
 * @param ctx a test context
 * @return the number of tests that failed.
 */
u32 but_get_test_failure_count(BUTContext *ctx);

/**
 * @brief retrieve the number of tests where the setup failed.
 *
 * @param ctx a test context
 * @return the number of tests where the setup failed.
 */
u32 but_get_setup_failure_count(BUTContext *ctx);

/**
 * @brief retrieve the number of tests that failed in the cleanup phase.
 *
 * @param ctx a test context.
 * @return the number of tests that failed in the cleanup phase.
 */
u32 but_get_cleanup_failure_count(BUTContext *ctx);

/**
 * @brief retrieve the number of result contexts.
 *
 * @param ctx a test context.
 * @return the number of result contexts.
 */
u32 but_get_results_count(BUTContext *ctx);

/**
 * @brief retrieve the result code from a result context.
 *
 * @param ctx a test context.
 * @param index the index of a result context.
 * @return a test-result code.
 */
BUTResultCode but_get_result(BUTContext *ctx, u32 index);

#if defined(__cplusplus)
}
#endif

#endif // BUT_CONTEXT_H_
