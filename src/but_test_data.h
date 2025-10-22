#ifndef BUT_TEST_DATA_H_
#define BUT_TEST_DATA_H_

/**
 * @file but_test_data.h
 * @author Douglas Cuthbertson
 * @brief Definitions for testing the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "but_context.h" // BUTContext

#include <but.h>               // BUTTestSuite
#include <abbreviated_types.h> // u32
#include <but_macros.h>        // DLL_SPEC

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
#define RUN_CURRENT_CTX_STR          "test_data_run_test"
#define GET_PASS_COUNT_CTX_STR       "test_data_get_pass_count"
#define GET_FAIL_COUNT_CTX_STR       "test_data_get_fail_count"
#define GET_SETUP_FAIL_COUNT_CTX_STR "test_data_get_setup_fail_count"
#define GET_RESULTS_COUNT_CTX_STR    "test_data_get_results_count"
#define GET_RESULT_CTX_STR           "test_data_get_result"
#define GET_CONTEXT                  "but_get_exception_context"
#define SET_CONTEXT                  "but_set_exception_context"

DLL_SPEC BUT_IS_VALID(test_data_is_valid);
DLL_SPEC BUT_INITIALIZE(test_data_initialize);
DLL_SPEC BUT_BEGIN(test_data_begin);
DLL_SPEC BUT_END(test_data_end);
DLL_SPEC BUT_NEXT(test_data_next);
DLL_SPEC BUT_HAS_MORE(test_data_more);
DLL_SPEC BUT_GET_TEST_CASE_NAME(test_data_get_test_case_name);
DLL_SPEC BUT_GET_INDEX(test_data_get_index);
DLL_SPEC BUT_DRIVER(test_data_run_test);
DLL_SPEC BUT_GET_RUN_COUNT(test_data_get_run_count);
DLL_SPEC BUT_GET_PASS_COUNT(test_data_get_pass_count);
DLL_SPEC BUT_GET_SETUP_FAILURE_COUNT(test_data_get_fail_count);
DLL_SPEC BUT_GET_SETUP_FAILURE_COUNT(test_data_get_setup_fail_count);
DLL_SPEC BUT_GET_RESULTS_COUNT(test_data_get_results_count);
DLL_SPEC BUT_GET_RESULT(test_data_get_result);

#endif // BUT_TEST_DATA_H_
