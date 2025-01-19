#ifndef TEST_DATA_H_
#define TEST_DATA_H_

/**
 * @file test_data.h
 * @author Douglas Cuthbertson
 * @brief Definitions for testing the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <but/context.h> // BUTContext
#include <but/but.h>     // BUTTestSuite
#include <platform.h>    // u32
#include <project.h>     // DATA_SPEC

#include <stdbool.h> // bool

// GetProcAddress never uses wchar_t, so these are regular C-strings
#define IS_VALID_CTX_STR             "test_data_is_valid"
#define INITIALIZE_CTX_STR           "test_data_initialize"
#define BEGIN_CTX_STR                "test_data_begin"
#define END_CTX_STR                  "test_data_end"
#define NEXT_CTX_STR                 "test_data_next"
#define MORE_CASES_CTX_STR           "test_data_more"
#define GET_CASE_NAME_CTX_STR        "test_data_get_test_case_name"
#define GET_CASE_INDEX_CTX_STR       "test_data_get_index"
#define RUN_CURRENT_CTX_STR          "test_data_test"
#define GET_PASS_COUNT_CTX_STR       "test_data_get_pass_count"
#define GET_FAIL_COUNT_CTX_STR       "test_data_get_fail_count"
#define GET_SETUP_FAIL_COUNT_CTX_STR "test_data_get_setup_fail_count"
#define GET_RESULTS_COUNT_CTX_STR    "test_data_get_results_count"
#define GET_RESULT_CTX_STR           "test_data_get_result"
#define REGISTER_CLOSURE             "exm_register_closure"

DATA_SPEC bool          test_data_is_valid(BUTContext *ctx);
DATA_SPEC void          test_data_begin(BUTContext *ctx, BUTTestSuite *bts);
DATA_SPEC void          test_data_end(BUTContext *ctx);
DATA_SPEC void          test_data_next(BUTContext *ctx);
DATA_SPEC bool          test_data_more(BUTContext *ctx);
DATA_SPEC char const   *test_data_get_test_case_name(BUTContext *ctx, BUTTestSuite *);
DATA_SPEC u32           test_data_get_index(BUTContext *ctx);
DATA_SPEC void          test_data_test(BUTContext *ctx);
DATA_SPEC u32           test_data_get_run_count(BUTContext *ctx);
DATA_SPEC u32           test_data_get_pass_count(BUTContext *ctx);
DATA_SPEC u32           test_data_get_fail_count(BUTContext *ctx);
DATA_SPEC u32           test_data_get_setup_fail_count(BUTContext *ctx);
DATA_SPEC u32           test_data_get_results_count(BUTContext *ctx);
DATA_SPEC BUTResultCode test_data_get_result(BUTContext *ctx, u32 index);

#endif // TEST_DATA_H_
