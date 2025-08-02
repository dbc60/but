/**
 * @file butts.c
 * @author Douglas Cuthbertson
 * @brief Test suite for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "but_driver.c"
#include "but_result_context.c"
#include "but_test.c"
#include "exception_assert.c"
#include "exception.c"

BUT_SUITE_BEGIN(driver)
BUT_SUITE_ADD_EMBEDDED(load_driver)
BUT_SUITE_ADD_EMBEDDED(begin_end)
BUT_SUITE_ADD_EMBEDDED(is_valid)
BUT_SUITE_ADD_EMBEDDED(next_index)
BUT_SUITE_ADD_EMBEDDED(case_name)
BUT_SUITE_ADD_EMBEDDED(case_index)
BUT_SUITE_ADD_EMBEDDED(test)
BUT_SUITE_ADD_EMBEDDED(results)
BUT_SUITE_END;
BUT_GET_TEST_SUITE("BUT Driver", driver)

// BUT_GLOBAL BUTTestCase *tca[]
//     = {&load_driver_test_case.tc,    &begin_end_test_case.tc,       &is_valid_test_case.tc, &next_index_test_case.tc,
//        &test_case_name_test_case.tc, &test_case_index_test_case.tc, &test_test_case.tc,     &results_test_case.tc};
//
// BUT_GLOBAL BUTTestSuite but_ts = {"BUT Driver", sizeof tca / sizeof(BUTTestCase *), tca};
//
// DLL_SPEC_EXPORT BUTTestSuite *get_test_suite() {
//     return &but_ts;
// }
