/**
 * @file test_data.c
 * @author Douglas Cuthbertson
 * @brief Test data for testing the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <but/context.h> // BUTContext
#include <but/but.h>     // BUTTestSuite
#include <platform.h>    // u32
#include <project.h>     // DATA_SPEC_EXPORT

#include <stdbool.h> // bool

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

// Test wrappers around the driver routines that are exported from the DLL
DATA_SPEC_EXPORT bool test_data_is_valid(BUTContext *ctx) {
    return but_is_valid(ctx);
}

DATA_SPEC_EXPORT void test_data_initialize(BUTContext *ctx) {
    but_initialize(ctx);
}

DATA_SPEC_EXPORT void test_data_begin(BUTContext *ctx, BUTTestSuite *bts) {
    but_begin(ctx, bts);
}

DATA_SPEC_EXPORT void test_data_end(BUTContext *ctx) {
    but_end(ctx);
}

DATA_SPEC_EXPORT void test_data_next(BUTContext *ctx) {
    but_next(ctx);
}

DATA_SPEC_EXPORT bool test_data_more(BUTContext *ctx) {
    return but_has_more(ctx);
}

DATA_SPEC_EXPORT const char *test_data_get_test_case_name(BUTContext *ctx) {
    return but_get_test_case_name(ctx);
}

DATA_SPEC_EXPORT u32 test_data_get_index(BUTContext *ctx) {
    return but_get_index(ctx);
}

DATA_SPEC_EXPORT void test_data_test(BUTContext *ctx) {
    but_test(ctx);
}

DATA_SPEC_EXPORT u32 test_data_get_run_count(BUTContext *ctx) {
    return but_get_run_count(ctx);
}

DATA_SPEC_EXPORT u32 test_data_get_pass_count(BUTContext *ctx) {
    return but_get_pass_count(ctx);
}

DATA_SPEC_EXPORT u32 test_data_get_fail_count(BUTContext *ctx) {
    return but_get_test_failure_count(ctx);
}

DATA_SPEC_EXPORT u32 test_data_get_setup_fail_count(BUTContext *ctx) {
    return but_get_setup_failure_count(ctx);
}

DATA_SPEC_EXPORT u32 test_data_get_results_count(BUTContext *ctx) {
    return but_get_results_count(ctx);
}

DATA_SPEC_EXPORT BUTResultCode test_data_get_result(BUTContext *ctx, u32 index) {
    return but_get_result(ctx, index);
}
