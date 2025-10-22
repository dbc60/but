#ifndef BUT_TEST_H_
#define BUT_TEST_H_

/**
 * @file but_test.h
 * @author Douglas Cuthbertson
 * @brief Test cases for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "but_driver.h"

#include <but.h> // BUTTestCase, BUTTestSuite, etc.

#include <stddef.h>  // size_t
#include <stdbool.h> // bool

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h> // HMODULE

// Function pointer typedefs for the BUT API
typedef void (*begin_fn)(BUTContext *bctx, BUTTestSuite *bts);
typedef void (*end_fn)(BUTContext *bctx);
typedef void (*next_fn)(BUTContext *bctx);
typedef bool (*has_more_fn)(BUTContext *bctx);
typedef char const *(*get_test_case_name_fn)(BUTContext *bctx, BUTTestSuite *bts);
typedef size_t (*get_index_fn)(BUTContext *bctx);
typedef void (*test_fn)(BUTContext *bctx, BUTTestCase *tc);
typedef size_t (*get_pass_count_fn)(BUTContext *bctx);
typedef size_t (*get_fail_count_fn)(BUTContext *bctx);
typedef size_t (*get_set_up_fail_count_fn)(BUTContext *bctx);
typedef size_t (*get_results_count_fn)(BUTContext *bctx);
typedef BUTResultCode (*get_result_fn)(BUTContext *bctx, size_t index);

// Test driver's data
struct TestDriverData {
    HMODULE                       h;
    but_get_exception_context_fn *get_context;
    but_set_exception_context_fn *set_context;
    BUTContext                    context;
    BUTTestCase                   btc;
    BUTTestSuite                 *bts;
    but_initialize_fn            *initialize_context;
    begin_fn                      begin;
    end_fn                        end;
    but_is_valid_fn              *is_valid;
    next_fn                       next;
    has_more_fn                   more;
    get_test_case_name_fn         get_test_case_name;
    get_index_fn                  get_index;
    test_fn                       test;
    get_pass_count_fn             get_pass_count;
    get_fail_count_fn             get_fail_count;
    get_set_up_fail_count_fn      get_failed_set_up_count;
    get_results_count_fn          get_results_count;
    get_result_fn                 get_result;
    void (*throw_me)(BUTContext *ctx);
};
typedef struct TestDriverData TestDriverData;

extern TestDriverData load_driver_case;
extern TestDriverData begin_end_case;
extern TestDriverData is_valid_case;
extern TestDriverData next_index_case;
extern TestDriverData test_case_name_case;
extern TestDriverData test_case_index_case;
extern TestDriverData test_case;
extern TestDriverData results_case;

#endif // BUT_TEST_H_
