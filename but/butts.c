/**
 * @file butts.c
 * @author Douglas Cuthbertson
 * @brief Test suite for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "test.h"

#include <but/context.h>
#include <but/but.h> // BUTTestCase, BUTTestSuite
#include <exceptions/types.h>
#include <project.h> // DATA_SPEC_EXPORT
#include <size.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

GLOBAL_VARIABLE BUTTestCase *tca[]
    = {&load_driver_test_case.tc,    &begin_end_test_case.tc,
       &is_valid_test_case.tc,       &next_index_test_case.tc,
       &test_case_name_test_case.tc, &test_case_index_test_case.tc,
       &test_test_case.tc,           &results_test_case.tc};

GLOBAL_VARIABLE BUTTestSuite but_ts
    = {"BUT Driver", sizeof tca / sizeof(BUTTestCase *), tca};

DATA_SPEC_EXPORT BUTTestSuite *but_test_suite() {
    return &but_ts;
}
