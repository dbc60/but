#ifndef TEST_H_
#define TEST_H_

/**
 * @file test.h
 * @author Douglas Cuthbertson
 * @brief Test cases for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <but/context.h> // BUTContext
#include <but/but.h>     // BUTTestCase, BUTTestSuite, etc.

#include <stddef.h>  // size_t
#include <stdbool.h> // bool

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h> // HMODULE

// Function pointer typedefs for the BUT API
typedef bool (*is_valid_fn)(BUTContext *ctx);
typedef void (*initialize_fn)(BUTContext *ctx);
typedef void (*begin_fn)(BUTContext *ctx, BUTTestSuite *bts);
typedef void (*end_fn)(BUTContext *ctx);
typedef void (*next_fn)(BUTContext *ctx);
typedef bool (*has_more_fn)(BUTContext *ctx);
typedef const char *(*get_test_case_name_fn)(BUTContext *ctx, BUTTestSuite *bts);
typedef size_t (*get_index_fn)(BUTContext *ctx);
typedef void (*test_fn)(BUTContext *ctx, BUTTestCase *tc);
typedef size_t (*get_pass_count_fn)(BUTContext *ctx);
typedef size_t (*get_fail_count_fn)(BUTContext *ctx);
typedef size_t (*get_setup_fail_count_fn)(BUTContext *ctx);
typedef size_t (*get_results_count_fn)(BUTContext *ctx);
typedef BUTResultCode (*get_result_fn)(BUTContext *ctx, size_t index);

// Test driver's data
struct TestDriverData {
    HMODULE                 h;
    exm_register_closure_fn register_closure;
    BUTContext              context;
    BUTTestCase             tc;
    BUTTestSuite           *ts;
    initialize_fn           initialize_context;
    begin_fn                begin;
    end_fn                  end;
    is_valid_fn             is_valid;
    next_fn                 next;
    has_more_fn             more;
    get_test_case_name_fn   get_test_case_name;
    get_index_fn            get_index;
    test_fn                 test;
    get_pass_count_fn       get_pass_count;
    get_fail_count_fn       get_fail_count;
    get_setup_fail_count_fn get_failed_setup_count;
    get_results_count_fn    get_results_count;
    get_result_fn           get_result;
};
typedef struct TestDriverData TestDriverData;

extern TestDriverData load_driver_test_case;
extern TestDriverData begin_end_test_case;
extern TestDriverData is_valid_test_case;
extern TestDriverData next_index_test_case;
extern TestDriverData test_case_name_test_case;
extern TestDriverData test_case_index_test_case;
extern TestDriverData test_test_case;
extern TestDriverData results_test_case;

#endif // TEST_H_
