/**
 * @file test_butts.c
 * @author Douglas Cuthbertson
 * @brief Basic Unit Test (BUT) test cases.
 * @version 0.1
 * @date 2024-11-04
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "test.h" // the test cases

#include <but/but.h> // BUTTestCase, BUTTestSuite
#include <project.h> // GLOBAL_VARIABLE, DATA_SPEC_EXPORT

GLOBAL_VARIABLE BUTTestCase *tca[]
    = {&load_driver_test_case.test_case,    &begin_end_test_case.test_case,
       &is_valid_test_case.test_case,       &next_index_test_case.test_case,
       &test_case_name_test_case.test_case, &test_case_index_test_case.test_case,
       &test_test_case.test_case,           &results_test_case.test_case};

GLOBAL_VARIABLE BUTTestSuite but_ts
    = {"BUT Driver", sizeof tca / sizeof(BUTTestCase *), tca};

DATA_SPEC_EXPORT BUTTestSuite *but_test_suite() {
    return &but_ts;
}
