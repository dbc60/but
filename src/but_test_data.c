/**
 * @file but_test_data.c
 * @author Douglas Cuthbertson
 * @brief Test data for testing the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "but_driver.c"
#include "but_result_context.c"
#include "exception_assert.c"
#include "exception.c"

DLL_SPEC_EXPORT BUT_HANDLER_FN(test_data_handler) {
    BUTContext *bctx = BUT_CONTAINER(ctx, BUTContext, but_ctx);

    if (BUT_UNEXPECTED_EXCEPTION(reason)) {
        BUTTestSuite *bts = bctx->env.bts;
        char         *name;
        if (bctx->env.index < bts->count) {
            name = bts->test_cases[bctx->env.index]->name;
        } else {
            name = "No test case";
        }

        if (details != NULL) {
            printf("  test_data_handler: Unexpected Exception. %s: %s, %s, @%s:%d\n", name, reason, details, file, line);
        } else {
            printf("  test_data_handler: Unexpected Exception. %s: %s, @%s:%d\n", name, reason, file, line);
        }
    }
}

// Test wrappers around the driver routines that are exported from the DLL
DLL_SPEC_EXPORT BUT_IS_VALID(test_data_is_valid) {
    return but_is_valid(bctx);
}

DLL_SPEC_EXPORT BUT_INITIALIZE(test_data_initialize) {
    but_initialize(bctx, handler);
}

DLL_SPEC_EXPORT BUT_BEGIN(test_data_begin) {
    but_begin(bctx, bts);
}

DLL_SPEC_EXPORT BUT_END(test_data_end) {
    but_end(bctx);
}

DLL_SPEC_EXPORT BUT_NEXT(test_data_next) {
    but_next(bctx);
}

DLL_SPEC_EXPORT BUT_HAS_MORE(test_data_more) {
    return but_has_more(bctx);
}

DLL_SPEC_EXPORT BUT_GET_TEST_CASE_NAME(test_data_get_test_case_name) {
    return but_get_test_case_name(bctx);
}

DLL_SPEC_EXPORT BUT_GET_INDEX(test_data_get_index) {
    return but_get_index(bctx);
}

DLL_SPEC_EXPORT BUT_DRIVER(test_data_run_test) {
    but_driver(bctx);
}

DLL_SPEC_EXPORT BUT_GET_RUN_COUNT(test_data_get_run_count) {
    return but_get_run_count(bctx);
}

DLL_SPEC_EXPORT BUT_GET_PASS_COUNT(test_data_get_pass_count) {
    return but_get_pass_count(bctx);
}

DLL_SPEC_EXPORT BUT_GET_SETUP_FAILURE_COUNT(test_data_get_fail_count) {
    return but_get_test_failure_count(bctx);
}

DLL_SPEC_EXPORT BUT_GET_SETUP_FAILURE_COUNT(test_data_get_setup_fail_count) {
    return but_get_setup_failure_count(bctx);
}

DLL_SPEC_EXPORT BUT_GET_RESULTS_COUNT(test_data_get_results_count) {
    return but_get_results_count(bctx);
}

DLL_SPEC_EXPORT BUT_GET_RESULT(test_data_get_result) {
    return but_get_result(bctx, index);
}
