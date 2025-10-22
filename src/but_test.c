/**
 * @file but_test.c
 * @author Douglas Cuthbertson
 * @brief Test cases for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "but_test.h" // TestDriverData
#include "log.h"

#include <but.h>             // BUTTestCase, BUTTestSuite
#include <but_assert.h>      // BUT_ASSERT_TRUE, BUT_ASSERT_FALSE, etc.
#include <exception_types.h> // but_handler_fn, BUTExceptionReason
#include <exception.h>       // BUT_THROW, BUT_TRY, BUT_CATCH, BUT_END_TRY, etc.
#include <but_macros.h>      // BUT_CONTAINER, BUT_GLOBAL, etc.

// We have to import the API defined in but_test_data.h
#undef BUT_DLL_BUILD
#include "but_test_data.h" // SET_CONTEXT, IS_VALID_CTX_STR, INITIALIZE_CTX_STR, etc.
#define BUT_DLL_BUILD

#include <stddef.h> // size_t, NULL
#include <stdio.h>  // printf

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h> // HMODULE, LoadLibrary, FreeLibrary

// Load data DLL. Note: this must be the name of the DLL for the test data.
// I haven't figured out a way to pass in this name, so it must be hard coded.
#define DRIVER_LIBRARY_STR  "but_test_data.dll"
#define DRIVER_LIBRARY_WSTR (L"" DRIVER_LIBRARY_STR)

#define TEST_SUCCESS "Success"
#define TEST_FAILURE "Failure"

static void set_up_context(BUTTestCase *btc);
static void set_up_test_context(BUTTestCase *btc);
static void cleanup_context(BUTTestCase *btc);

static void set_up_test_driver_data(TestDriverData *tc);
static void cleanup_test_driver_data(TestDriverData *tc);

static BUT_HANDLER_FN(test_handler) {
    BUTContext *bctx = BUT_CONTAINER(ctx, BUTContext, exception_context);
    if (BUT_UNEXPECTED_EXCEPTION(reason)) {
        BUTTestSuite *bts = bctx->env.bts;
        BUTTestCase  *btc = bts->test_cases[bctx->env.index];

        file = logger_get_filename(file);
        if (details != NULL) {
            LOG_ERROR("test handler", "%s: %s, %s, %s:%d", btc->name, reason, details,
                      file, line);
        } else {
            LOG_ERROR("test handler", "%s: %s, %s:%d", btc->name, reason, file, line);
        }
    }
}

BUT_TYPE_TEST_SETUP_CLEANUP(TEST_SUCCESS, TestDriverData, success, NULL, NULL) {
    BUT_UNUSED_TYPE_ARG;
    return; // This test always succeeds.
}

// The failure should be caught in test() below.
BUT_TYPE_TEST_SETUP_CLEANUP(TEST_FAILURE, TestDriverData, failure, NULL, NULL) {
    BUT_UNUSED_TYPE_ARG;
    BUT_THROW(but_test_exception);
}

BUT_SUITE_BEGIN(driver_test_data)
BUT_SUITE_ADD_EMBEDDED(success)
BUT_SUITE_ADD_EMBEDDED(failure)
BUT_SUITE_END;

BUT_TEST_SUITE("Driver Data", driver_test_data);

BUT_TYPE_TEST_SETUP_CLEANUP("Load Driver", TestDriverData, load_driver, NULL, NULL) {
    BUT_UNUSED_TYPE_ARG;
    HMODULE library = LoadLibrary(DRIVER_LIBRARY_WSTR);

    BUT_ASSERT_TRUE(library != 0);
    FreeLibrary(library);
}

void dbg_test_throw(BUTContext *exception_context) {
    LOG_DEBUG("Debug Test Throw", "context=0x%p, stack=0x%p",
              &exception_context->exception_context,
              exception_context->exception_context.stack);
    (void)but_get_exception_context(__FILE__, __LINE__);
    LOG_DEBUG("Debug Test Throw", "throwing but_test_exception");
    BUT_THROW(but_test_exception);
}

static void set_up_test_driver_data(TestDriverData *tdd) {
    tdd->h = LoadLibrary(DRIVER_LIBRARY_WSTR);
    BUT_ASSERT_NOT_NULL(tdd->h);

    tdd->get_context
        = (but_get_exception_context_fn *)GetProcAddress(tdd->h, GET_CONTEXT);
    BUT_ASSERT_NOT_NULL(tdd->get_context);

    tdd->set_context
        = (but_set_exception_context_fn *)GetProcAddress(tdd->h, SET_CONTEXT);
    BUT_ASSERT_NOT_NULL(tdd->set_context);

    tdd->is_valid = (but_is_valid_fn *)GetProcAddress(tdd->h, IS_VALID_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->is_valid);

    tdd->initialize_context
        = (but_initialize_fn *)GetProcAddress(tdd->h, INITIALIZE_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->initialize_context);

    tdd->begin = (begin_fn)GetProcAddress(tdd->h, BEGIN_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->begin);

    tdd->end = (end_fn)GetProcAddress(tdd->h, END_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->end);

    tdd->next = (next_fn)GetProcAddress(tdd->h, NEXT_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->next);

    tdd->more = (has_more_fn)GetProcAddress(tdd->h, MORE_CASES_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->more);

    tdd->get_test_case_name
        = (get_test_case_name_fn)GetProcAddress(tdd->h, GET_CASE_NAME_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->get_test_case_name);

    tdd->get_index = (get_index_fn)GetProcAddress(tdd->h, GET_CASE_INDEX_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->get_index);

    tdd->test = (test_fn)GetProcAddress(tdd->h, RUN_CURRENT_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->test);

    tdd->get_pass_count
        = (get_pass_count_fn)GetProcAddress(tdd->h, GET_PASS_COUNT_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->get_pass_count);

    tdd->get_fail_count
        = (get_fail_count_fn)GetProcAddress(tdd->h, GET_FAIL_COUNT_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->get_fail_count);

    tdd->get_failed_set_up_count
        = (get_set_up_fail_count_fn)GetProcAddress(tdd->h, GET_SETUP_FAIL_COUNT_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->get_failed_set_up_count);

    tdd->get_results_count
        = (get_results_count_fn)GetProcAddress(tdd->h, GET_RESULTS_COUNT_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->get_results_count);

    tdd->get_result = (get_result_fn)GetProcAddress(tdd->h, GET_RESULT_CTX_STR);
    BUT_ASSERT_NOT_NULL(tdd->get_result);

    tdd->bts      = &BUT_TEST_SUITE_NAME(driver_test_data);
    tdd->throw_me = dbg_test_throw;
}

static void cleanup_test_driver_data(TestDriverData *tdd) {
    if (tdd->h != NULL) {
        FreeLibrary(tdd->h);
        tdd->h = NULL;
    }
}

static void set_up_context(BUTTestCase *btc) {
    TestDriverData *tdd = BUT_CONTAINER(btc, TestDriverData, btc);
    set_up_test_driver_data(tdd);
    but_handler handler = (but_handler)GetProcAddress(tdd->h, "test_data_handler");
    if (handler == NULL) {
        BUT_THROW("failed to load test_data_handler");
    }
    tdd->initialize_context(&tdd->context, handler);
    tdd->begin(&tdd->context, tdd->bts);

    if (!tdd->is_valid(&tdd->context)) {
        cleanup_test_driver_data(tdd);
    }
}

static void set_up_test_context(BUTTestCase *btc) {
    TestDriverData *tdd = BUT_CONTAINER(btc, TestDriverData, btc);
    set_up_test_driver_data(tdd);
    tdd->initialize_context(&tdd->context, test_handler);
    (void)tdd->get_context(__FILE__, __LINE__);
    (void)but_get_exception_context(__FILE__, __LINE__);
    tdd->begin(&tdd->context, tdd->bts);

    if (!tdd->is_valid(&tdd->context)) {
        cleanup_test_driver_data(tdd);
    }
    tdd->context.exception_context.handler = test_handler;
    tdd->set_context(&tdd->context.exception_context, __FILE__, __LINE__);
    (void)tdd->get_context(__FILE__, __LINE__);
    (void)but_get_exception_context(__FILE__, __LINE__);
}

static void cleanup_context(BUTTestCase *btc) {
    TestDriverData *tdd = BUT_CONTAINER(btc, TestDriverData, btc);
    tdd->end(&tdd->context);
    cleanup_test_driver_data(tdd);
}

// Verify begin() and end() create and delete the test context
BUT_TYPE_TEST_SETUP_CLEANUP("Begin and End", TestDriverData, begin_end, set_up_context,
                            cleanup_context) {
    t->end(&t->context);
    BUT_ASSERT_NULL(t->context.env.results);
}

// Verify the context is valid
BUT_TYPE_TEST_SETUP_CLEANUP("Is Valid", TestDriverData, is_valid, set_up_context,
                            cleanup_context) {
    BUT_ASSERT_TRUE(t->is_valid(&t->context));
    t->end(&t->context);
}

// Verify get_index(), more(), and next() work as expected
BUT_TYPE_TEST_SETUP_CLEANUP("Next/Index/More", TestDriverData, next_index,
                            set_up_context, cleanup_context) {
    // verify the first index is zero
    BUT_ASSERT_TRUE(t->get_index(&t->context) == 0);

    // verify next() advances the index to 1
    t->next(&t->context);
    BUT_ASSERT_EQ_SIZE_T(t->get_index(&t->context), (size_t)1);

    // verify more() returns true
    BUT_ASSERT_TRUE(t->more(&t->context));

    t->next(&t->context);
    // There should be no more test cases
    BUT_ASSERT_FALSE(t->more(&t->context));
}

// Verify get_test_case_name returns the correct names of test cases
BUT_TYPE_TEST_SETUP_CLEANUP("Case Name", TestDriverData, case_name, set_up_context,
                            cleanup_context) {
    char const *name;

    // The first test case should be "Success"
    name = t->get_test_case_name(&t->context, t->bts);
    BUT_ASSERT_STREQ(name, TEST_SUCCESS);

    // Advance to the next test case
    t->next(&t->context);

    // The second test case should be "Failure"
    name = t->get_test_case_name(&t->context, t->bts);
    BUT_ASSERT_STREQ(name, TEST_FAILURE);
}

BUT_TYPE_TEST_SETUP_CLEANUP("Case Index", TestDriverData, case_index, set_up_context,
                            cleanup_context) {
    size_t index;

    // The first test case should have an index of 0
    index = t->get_index(&t->context);
    BUT_ASSERT_TRUE(index == 0);

    // Advance to the next test case
    t->next(&t->context);

    // The second test case should have an index of 1
    index = t->get_index(&t->context);
    BUT_ASSERT_EQ_SIZE_T(index, (size_t)1);
}

BUT_TYPE_TEST_SETUP_CLEANUP("Test the Driver", TestDriverData, test, set_up_test_context,
                            cleanup_context) {
    // Verify the test case is valid
    BUT_ASSERT_TRUE(t->is_valid(&t->context));

    BUT_TRY {
        t->test(&t->context, t->context.env.bts->test_cases[t->context.env.index]);
        t->next(&t->context);
        t->test(&t->context, t->context.env.bts->test_cases[t->context.env.index]);
        // Should have thrown but_expected_failure to be caught below
        BUT_THROW(but_internal_error);
    }
    BUT_CATCH(but_test_exception) {
        // Catch and update counters
        t->context.env.test_failures++;
        t->context.env.results_count++;
        t->context.env.run_count++;
        (void)but_get_exception_context(__FILE__, __LINE__);
    }
    // If anything else was thrown, it's a failure. Let the test driver catch it.
    BUT_END_TRY;

    BUT_ASSERT_EQ_SIZE_T(t->get_pass_count(&t->context), (size_t)1);
    BUT_ASSERT_EQ_SIZE_T(t->get_fail_count(&t->context), (size_t)1);
    BUT_ASSERT_TRUE(t->get_failed_set_up_count(&t->context) == 0);
    BUT_ASSERT_EQ_UINT32(t->context.env.results_count, 1);
}

BUT_TYPE_TEST_SETUP_CLEANUP("Results", TestDriverData, results, set_up_test_context,
                            cleanup_context) {
    t->test(&t->context, t->context.env.bts->test_cases[t->context.env.index]);
    t->next(&t->context);
    BUT_TRY {
        t->test(&t->context, t->context.env.bts->test_cases[t->context.env.index]);
        // Should have failed
        BUT_THROW(but_test_exception);
    }
    BUT_CATCH(but_test_exception) {
        ; // Catch and release
    }
    BUT_END_TRY;
}
