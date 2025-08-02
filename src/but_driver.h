#ifndef BUT_DRIVER_H_
#define BUT_DRIVER_H_

/**
 * @file but_driver.h
 * @author Douglas Cuthbertson
 * @brief context definitions for the Basic Unit Test (BUT) module.
 * @version 0.1
 * @date 2023-12-09
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "but_context.h" // BUTContext

#include <but.h>               // BUTTestCase and BUTTestSuite
#include <exception_types.h>   // BUTExceptionContext
#include <abbreviated_types.h> // u32

#include <stdbool.h> // bool

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Note that but_is_valid is declared using an experimental form for defining an
 * interface for the test driver. If used correctly, it ensures that implementations of
 * this function and pointers to them are the same type. For example, but_test.h uses
 * pointers to functions that are supposed to be identical to the BUT API. If instead of
 * defining its own pointer types, it uses pointers to the declared function types, then
 * the pointers are guaranteed to be the correct type.
 *
 * Also, the macros can be used to define the function itself; see the definition of
 * but_is_valid in but_driver.c, for example.
 */

/**
 * @brief determine if the context has been properly initialized by calls to
 * but_initialize and but_begin.
 *
 * @param bctx a test context
 * @return true if the test context is valid and false otherwise.
 */
#define BUT_IS_VALID(name) bool name(BUTContext *bctx)
typedef BUT_IS_VALID(but_is_valid_fn);
BUT_IS_VALID(but_is_valid);

/**
 * @brief initialize a test context.
 *
 * @param bctx the test context to be initialized.
 */
#define BUT_INITIALIZE(name) void name(BUTContext *bctx, but_handler handler)
typedef BUT_INITIALIZE(but_initialize_fn);
BUT_INITIALIZE(but_initialize);

/**
 * @brief but_begin assigns a test suite to a test context.
 *
 * @param bctx a test context.
 * @param bts a test suite.
 */
#define BUT_BEGIN(name) void name(BUTContext *bctx, struct BUTTestSuite *bts)
typedef BUT_BEGIN(but_begin_fn);
BUT_BEGIN(but_begin);

/**
 * @brief but_end releases the memory resources allocated during testing.
 *
 * @param bctx test context
 */
#define BUT_END(name) void name(BUTContext *bctx)
typedef BUT_END(but_end_fn);
BUT_END(but_end);

/**
 * @brief but_next is part of the iterator interface. If there is another test case in
 * the test suite, the context is updated to hold that test case as the current one. If
 * there are no more test cases, the context is unchanged.
 *
 * @param bctx a test context.
 */
#define BUT_NEXT(name) void name(BUTContext *bctx)
typedef BUT_NEXT(but_next_fn);
BUT_NEXT(but_next);

/**
 * @brief but_has_more is part of the iterator interface. It determines if there is
 * another test case in the test suite,
 *
 * @param cts a test context.
 *
 * @return true if there are more test cases after the current one, and false otherwise.
 */
#define BUT_HAS_MORE(name) bool name(BUTContext *bctx)
typedef BUT_HAS_MORE(but_has_more_fn);
BUT_HAS_MORE(but_has_more);

/**
 * @brief but_get_test_case_name retrieves the name of the current test case.
 *
 * @param bctx a test context.
 * @param bts a test suite.
 *
 * @return the name of the current test case.
 */
#define BUT_GET_TEST_CASE_NAME(name) char const *name(BUTContext *bctx)
typedef BUT_GET_TEST_CASE_NAME(but_get_test_case_name_fn);
BUT_GET_TEST_CASE_NAME(but_get_test_case_name);

/**
 * @brief retrieve the index of the current test case.
 *
 * @param bctx a test context.
 * @return the zero-based index of the test case.
 */
#define BUT_GET_INDEX(name) u32 name(BUTContext *bctx)
typedef BUT_GET_INDEX(but_get_index_fn);
BUT_GET_INDEX(but_get_index);

/**
 * @brief but_driver executes the current test case.
 *
 * @param bctx a test context.
 */
#define BUT_DRIVER(name) void name(BUTContext *bctx)
typedef BUT_DRIVER(but_driver_fn);
BUT_DRIVER(but_driver);

/**
 * @brief retrieve the number of test cases executed.
 *
 * @param bctx a test context.
 * @return the number of test cases executed.
 */
#define BUT_GET_RUN_COUNT(name) u32 name(BUTContext *bctx)
typedef BUT_GET_RUN_COUNT(but_get_run_count_fn);
BUT_GET_RUN_COUNT(but_get_run_count);

/**
 * @brief retrieve the number of tests that passed.
 *
 * @param bctx a test context.
 * @return  the number of tests that passed.
 */
#define BUT_GET_PASS_COUNT(name) u32 name(BUTContext *bctx)
typedef BUT_GET_PASS_COUNT(but_get_pass_count_fn);
BUT_GET_PASS_COUNT(but_get_pass_count);

/**
 * @brief retrieve the number of tests that failed.
 *
 * @param bctx a test context
 * @return the number of tests that failed.
 */
#define BUT_GET_TEST_FAILURE_COUNT(name) u32 name(BUTContext *bctx)
typedef BUT_GET_TEST_FAILURE_COUNT(but_get_test_failure_count_fn);
BUT_GET_TEST_FAILURE_COUNT(but_get_test_failure_count);

/**
 * @brief retrieve the number of tests where the setup failed.
 *
 * @param bctx a test context
 * @return the number of tests where the setup failed.
 */
#define BUT_GET_SETUP_FAILURE_COUNT(name) u32 name(BUTContext *bctx)
typedef BUT_GET_SETUP_FAILURE_COUNT(but_get_setup_failure_count_fn);
BUT_GET_SETUP_FAILURE_COUNT(but_get_setup_failure_count);

/**
 * @brief retrieve the number of tests that failed in the cleanup phase.
 *
 * @param bctx a test context.
 * @return the number of tests that failed in the cleanup phase.
 */
#define BUT_GET_CLEANUP_FAILURE_COUNT(name) u32 name(BUTContext *bctx)
typedef BUT_GET_CLEANUP_FAILURE_COUNT(but_get_cleanup_failure_count_fn);
BUT_GET_CLEANUP_FAILURE_COUNT(but_get_cleanup_failure_count);

/**
 * @brief retrieve the number of result contexts.
 *
 * @param bctx a test context.
 * @return the number of result contexts.
 */
#define BUT_GET_RESULTS_COUNT(name) u32 name(BUTContext *bctx)
typedef BUT_GET_RESULTS_COUNT(but_get_results_count_fn);
BUT_GET_RESULTS_COUNT(but_get_results_count);

/**
 * @brief retrieve the result code from a result context.
 *
 * @param bctx a test context.
 * @param index the index of a result context.
 * @return a test-result code.
 */
#define BUT_GET_RESULT(name) BUTResultCode name(BUTContext *bctx, u32 index)
typedef BUT_GET_RESULT(but_get_result_fn);
BUT_GET_RESULT(but_get_result);

#if defined(__cplusplus)
}
#endif

#endif // BUT_DRIVER_H_
