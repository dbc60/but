/**
 * @file test.c
 * @author Douglas Cuthbertson
 * @brief Test cases for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "test.h" // TestDriverData

#include <but/but.h>               // BUTTestCase, BUTTestSuite
#include <exceptions/types.h>      // exm_closure, EXMReason
#include <exceptions/exceptions.h> // EXM_THROW, EXM_TRY, EXM_CATCH, EXM_END_TRY, etc.
#include <project.h>               // CONTAINER_OF, GLOBAL_VARIABLE, etc.

// We have to import the API defined in test_data.h
#undef BUT_DLL_BUILD
#include "test_data.h" // REGISTER_CLOSURE, IS_VALID_CTX_STR, INITIALIZE_CTX_STR, etc.
#define BUT_DLL_BUILD

#include <stddef.h> // size_t, NULL
#include <stdio.h>  // printf

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h> // HMODULE, LoadLibrary, FreeLibrary

// Load data DLL. Note: this must be the name of the DLL for the test data.
// I haven't figured out a way to pass in this name, so it must be hard coded.
#define DRIVER_LIBRARY_STR  "test_data.dll"
#define DRIVER_LIBRARY_WSTR (L"" DRIVER_LIBRARY_STR)

#define TEST_NAME1 "Test Success"
#define TEST_NAME2 "Test Failure"

GLOBAL_VARIABLE EXMReason load_driver_failure
    = "failed to load driver library " DRIVER_LIBRARY_STR;
GLOBAL_VARIABLE EXMReason load_register_handler_failure
    = "failed to load " REGISTER_CLOSURE;
GLOBAL_VARIABLE EXMReason load_is_valid_failure = "failed to load " IS_VALID_CTX_STR;
GLOBAL_VARIABLE EXMReason load_initialization_failure
    = "failed to load " INITIALIZE_CTX_STR;
GLOBAL_VARIABLE EXMReason load_begin_failure    = "failed to load " BEGIN_CTX_STR;
GLOBAL_VARIABLE EXMReason load_end_failure      = "failed to load " END_CTX_STR;
GLOBAL_VARIABLE EXMReason load_next_failure     = "failed to load " NEXT_CTX_STR;
GLOBAL_VARIABLE EXMReason load_has_more_failure = "failed to load " MORE_CASES_CTX_STR;
GLOBAL_VARIABLE EXMReason load_get_test_case_name_failure
    = "failed to load " GET_CASE_NAME_CTX_STR;
GLOBAL_VARIABLE EXMReason load_get_index_failure
    = "failed to load " GET_CASE_INDEX_CTX_STR;
GLOBAL_VARIABLE EXMReason load_run_failure = "failed to load " RUN_CURRENT_CTX_STR;
GLOBAL_VARIABLE EXMReason load_get_pass_count_failure
    = "failed to load " GET_PASS_COUNT_CTX_STR;
GLOBAL_VARIABLE EXMReason load_get_fail_count_failure
    = "failed to load " GET_FAIL_COUNT_CTX_STR;
GLOBAL_VARIABLE EXMReason load_get_setup_fail_count_failure
    = {"failed to load " GET_SETUP_FAIL_COUNT_CTX_STR};
GLOBAL_VARIABLE EXMReason load_get_results_count_failure
    = {"failed to load " GET_RESULTS_COUNT_CTX_STR};
GLOBAL_VARIABLE EXMReason load_get_result_failure = "failed to load " GET_RESULT_CTX_STR;
GLOBAL_VARIABLE EXMReason clear_test_context_failure
    = "end failed to clear the test context";
GLOBAL_VARIABLE EXMReason invalid_context_failure   = "the context is not valid";
GLOBAL_VARIABLE EXMReason end_of_test_cases_failure = "expected no more test cases";
GLOBAL_VARIABLE EXMReason has_more_failure          = "expected more test cases";
GLOBAL_VARIABLE EXMReason second_index_failure      = "second index is not one";
GLOBAL_VARIABLE EXMReason first_index_failure       = "first index is not zero";
GLOBAL_VARIABLE EXMReason expected_test_case_name_is_not
    = "test case name is not " TEST_NAME1;
GLOBAL_VARIABLE EXMReason unexpected_test_case_name_is_not
    = "test case name is not " TEST_NAME2;
GLOBAL_VARIABLE EXMReason unexpected_index          = "unexpected test case index";
GLOBAL_VARIABLE EXMReason expected_one_passing_test = "expected one passing test";
GLOBAL_VARIABLE EXMReason expected_one_failing_test = "expected one failing test";
GLOBAL_VARIABLE EXMReason unexpected_setup_failure_count
    = "expected zero setup failures";
GLOBAL_VARIABLE EXMReason unexpected_test_result_count
    = "expected one non-passing test result";
GLOBAL_VARIABLE EXMReason test_throw_failure = "failed to throw test failure";

INTERNAL_FUNCTION void test_success(BUTTestCase *tc);
INTERNAL_FUNCTION void test_failure(BUTTestCase *tc);
INTERNAL_FUNCTION void test_load_driver(BUTTestCase *tc);
INTERNAL_FUNCTION void setup_context(BUTTestCase *tc);
INTERNAL_FUNCTION void setup_test_context(BUTTestCase *tc);
INTERNAL_FUNCTION void cleanup_context(BUTTestCase *tc);
INTERNAL_FUNCTION void test_begin_end(BUTTestCase *tc);
INTERNAL_FUNCTION void test_is_valid(BUTTestCase *tc);
INTERNAL_FUNCTION void test_nextIndex_more(BUTTestCase *tc);
INTERNAL_FUNCTION void test_case_name(BUTTestCase *tc);
INTERNAL_FUNCTION void test_index(BUTTestCase *tc);
INTERNAL_FUNCTION void test_test(BUTTestCase *tc);
INTERNAL_FUNCTION void test_result(BUTTestCase *tc);

INTERNAL_FUNCTION void setup_test_driver_data(TestDriverData *tc);
INTERNAL_FUNCTION void cleanup_test_driver_data(TestDriverData *tc);

TestDriverData load_driver_test_case = {
    .tc.name = "Load Driver",
    .tc.test = test_load_driver,
};

TestDriverData begin_end_test_case = {
    .tc.name    = "Begin and End",
    .tc.setup   = setup_context,
    .tc.test    = test_begin_end,
    .tc.cleanup = cleanup_context,
};

TestDriverData is_valid_test_case = {
    .tc.name    = "IsValid",
    .tc.setup   = setup_context,
    .tc.test    = test_is_valid,
    .tc.cleanup = cleanup_context,
};

TestDriverData next_index_test_case = {
    .tc.name    = "Next/Index/More",
    .tc.setup   = setup_context,
    .tc.test    = test_nextIndex_more,
    .tc.cleanup = cleanup_context,
};

TestDriverData test_case_name_test_case = {
    .tc.name    = "Case Name",
    .tc.setup   = setup_context,
    .tc.test    = test_case_name,
    .tc.cleanup = cleanup_context,
};

TestDriverData test_case_index_test_case = {
    .tc.name    = "Index",
    .tc.setup   = setup_context,
    .tc.test    = test_index,
    .tc.cleanup = cleanup_context,
};

TestDriverData test_test_case = {
    .tc.name    = "Test",
    .tc.setup   = setup_test_context,
    .tc.test    = test_test,
    .tc.cleanup = cleanup_context,
};

TestDriverData results_test_case = {
    .tc.name    = "Results",
    .tc.setup   = setup_test_context,
    .tc.test    = test_result,
    .tc.cleanup = cleanup_context,
};

INTERNAL_FUNCTION void test_handler(exm_closure *exm_closure, EXMReason reason,
                                    char const *details, char const *file, int line) {
    BUTContext *ctx = CONTAINER_OF(exm_closure, BUTContext, exm_ctx);
    if (BUT_UNEXPECTED_EXCEPTION(reason)) {
        BUTTestSuite *bts = ctx->env.bts;
        BUTTestCase  *btc = bts->test_cases[ctx->env.index];

        if (details != NULL) {
            printf("  test_handler %s: %s, %s, file %s: %d\n", btc->name, reason,
                   details, file, line);
        } else {
            printf("  test_handler %s: %s, file %s: %d\n", btc->name, reason, file,
                   line);
        }
        // We must thrown here, because this function was called directly from
        // exm_throw() rather than via a longjmp. The throw will longjmp to the
        // loop in the test driver instead of returning to exm_throw.
        EXM_THROW(reason);
    }
}

INTERNAL_FUNCTION void test_success(BUTTestCase *tc) {
    UNUSED_PARAMETER(tc);
}

// This should be caught in test_test below.
INTERNAL_FUNCTION void test_failure(BUTTestCase *tc) {
    UNUSED_PARAMETER(tc);
    EXM_THROW(exm_expected_failure);
}

GLOBAL_VARIABLE TestDriverData driver_test_case_success = {
    .tc.name = TEST_NAME1,
    .tc.test = test_success,
};

GLOBAL_VARIABLE TestDriverData driver_test_case_failure = {
    .tc.name = TEST_NAME2,
    .tc.test = test_failure,
};

GLOBAL_VARIABLE BUTTestCase *tca[]
    = {&driver_test_case_success.tc, &driver_test_case_failure.tc};

GLOBAL_VARIABLE BUTTestSuite test_suite
    = {"Driver", sizeof tca / sizeof(TestDriverData *), tca};

INTERNAL_FUNCTION void test_load_driver(BUTTestCase *tc) {
    HMODULE library;

    UNUSED_PARAMETER(tc);

    library = LoadLibrary(DRIVER_LIBRARY_WSTR);

    if (library) {
        FreeLibrary(library);
    } else {
        EXM_THROW(load_driver_failure);
    }
}

INTERNAL_FUNCTION void setup_test_driver_data(TestDriverData *tdd) {
    tdd->h = LoadLibrary(DRIVER_LIBRARY_WSTR);

    if (tdd->h == NULL) {
        EXM_THROW(load_driver_failure);
    }

    tdd->register_closure
        = (exm_register_closure_fn)GetProcAddress(tdd->h, REGISTER_CLOSURE);
    if (tdd->register_closure == NULL) {
        EXM_THROW(load_register_handler_failure);
    }

    tdd->is_valid = (is_valid_fn)GetProcAddress(tdd->h, IS_VALID_CTX_STR);
    if (tdd->is_valid == NULL) {
        EXM_THROW(load_is_valid_failure);
    }

    tdd->initialize_context = (initialize_fn)GetProcAddress(tdd->h, INITIALIZE_CTX_STR);
    if (tdd->initialize_context == NULL) {
        EXM_THROW(load_initialization_failure);
    }

    tdd->begin = (begin_fn)GetProcAddress(tdd->h, BEGIN_CTX_STR);
    if (tdd->begin == NULL) {
        EXM_THROW(load_begin_failure);
    }

    tdd->end = (end_fn)GetProcAddress(tdd->h, END_CTX_STR);
    if (tdd->end == NULL) {
        EXM_THROW(load_end_failure);
    }

    tdd->next = (next_fn)GetProcAddress(tdd->h, NEXT_CTX_STR);
    if (tdd->next == NULL) {
        EXM_THROW(load_next_failure);
    }

    tdd->more = (has_more_fn)GetProcAddress(tdd->h, MORE_CASES_CTX_STR);
    if (tdd->more == NULL) {
        EXM_THROW(load_has_more_failure);
    }

    tdd->get_test_case_name
        = (get_test_case_name_fn)GetProcAddress(tdd->h, GET_CASE_NAME_CTX_STR);
    if (tdd->get_test_case_name == NULL) {
        EXM_THROW(load_get_test_case_name_failure);
    }

    tdd->get_index = (get_index_fn)GetProcAddress(tdd->h, GET_CASE_INDEX_CTX_STR);
    if (tdd->get_index == NULL) {
        EXM_THROW(load_get_index_failure);
    }

    tdd->test = (test_fn)GetProcAddress(tdd->h, RUN_CURRENT_CTX_STR);
    if (tdd->test == NULL) {
        EXM_THROW(load_run_failure);
    }

    tdd->get_pass_count
        = (get_pass_count_fn)GetProcAddress(tdd->h, GET_PASS_COUNT_CTX_STR);
    if (tdd->get_pass_count == NULL) {
        EXM_THROW(load_get_pass_count_failure);
    }

    tdd->get_fail_count
        = (get_fail_count_fn)GetProcAddress(tdd->h, GET_FAIL_COUNT_CTX_STR);
    if (tdd->get_fail_count == NULL) {
        EXM_THROW(load_get_fail_count_failure);
    }

    tdd->get_failed_setup_count
        = (get_setup_fail_count_fn)GetProcAddress(tdd->h, GET_SETUP_FAIL_COUNT_CTX_STR);
    if (tdd->get_failed_setup_count == NULL) {
        EXM_THROW(load_get_setup_fail_count_failure);
    }

    tdd->get_results_count
        = (get_results_count_fn)GetProcAddress(tdd->h, GET_RESULTS_COUNT_CTX_STR);
    if (tdd->get_results_count == NULL) {
        EXM_THROW(load_get_results_count_failure);
    }

    tdd->get_result = (get_result_fn)GetProcAddress(tdd->h, GET_RESULT_CTX_STR);
    if (tdd->get_result == NULL) {
        EXM_THROW(load_get_result_failure);
    }

    tdd->ts = &test_suite;
}

INTERNAL_FUNCTION void cleanup_test_driver_data(TestDriverData *tdd) {
    if (tdd->h != NULL) {
        FreeLibrary(tdd->h);
        tdd->h = NULL;
    }
}

INTERNAL_FUNCTION void setup_context(BUTTestCase *tc) {
    TestDriverData *tdd = CONTAINER_OF(tc, TestDriverData, tc);
    setup_test_driver_data(tdd);
    tdd->initialize_context(&tdd->context);
    tdd->begin(&tdd->context, tdd->ts);

    if (!tdd->is_valid(&tdd->context)) {
        cleanup_test_driver_data(tdd);
    }
}

INTERNAL_FUNCTION void setup_test_context(BUTTestCase *tc) {
    TestDriverData *tdd = CONTAINER_OF(tc, TestDriverData, tc);
    setup_test_driver_data(tdd);
    tdd->initialize_context(&tdd->context);
    tdd->begin(&tdd->context, tdd->ts);

    if (!tdd->is_valid(&tdd->context)) {
        cleanup_test_driver_data(tdd);
    }
    tdd->context.exm_ctx.handler = test_handler;
    tdd->register_closure(&tdd->context.exm_ctx.handler);
}

INTERNAL_FUNCTION void cleanup_context(BUTTestCase *tc) {
    TestDriverData *tdd = CONTAINER_OF(tc, TestDriverData, tc);
    tdd->end(&tdd->context);
    cleanup_test_driver_data(tdd);
}

// Verify begin() and end() create and delete the test context
INTERNAL_FUNCTION void test_begin_end(BUTTestCase *tc) {
    TestDriverData *tdd = CONTAINER_OF(tc, TestDriverData, tc);

    tdd->end(&tdd->context);
    if (tdd->context.env.results != NULL) {
        EXM_THROW(clear_test_context_failure);
    }
}

// Verify the context is valid
INTERNAL_FUNCTION void test_is_valid(BUTTestCase *tc) {
    TestDriverData *tdd = CONTAINER_OF(tc, TestDriverData, tc);

    if (!tdd->is_valid(&tdd->context)) {
        EXM_THROW(invalid_context_failure);
    }
    tdd->end(&tdd->context);
}

// Verify more()
INTERNAL_FUNCTION void test_nextIndex_more(BUTTestCase *tc) {
    TestDriverData *tdd = CONTAINER_OF(tc, TestDriverData, tc);

    if (tdd->get_index(&tdd->context) == 0) {
        tdd->next(&tdd->context);
        if (tdd->get_index(&tdd->context) == 1) {
            if (tdd->more(&tdd->context)) {
                tdd->next(&tdd->context);

                // There should be no more test cases
                if (tdd->more(&tdd->context)) {
                    EXM_THROW(end_of_test_cases_failure);
                }
            } else {
                EXM_THROW(has_more_failure);
            }
        } else {
            EXM_THROW(second_index_failure);
        }
    } else {
        EXM_THROW(first_index_failure);
    }
}

// Verify get_test_case_name returns the correct names of test cases
INTERNAL_FUNCTION void test_case_name(BUTTestCase *tc) {
    TestDriverData *tdd   = CONTAINER_OF(tc, TestDriverData, tc);
    int             error = 0;
    char const     *name;

    name = tdd->get_test_case_name(&tdd->context, tdd->ts);

    error = strcmp(name, TEST_NAME1);
    if (error != 0) {
        EXM_THROW(expected_test_case_name_is_not);
    }

    tdd->next(&tdd->context);
    name  = tdd->get_test_case_name(&tdd->context, tdd->ts);
    error = strcmp(name, TEST_NAME2);

    if (error != 0) {
        EXM_THROW(unexpected_test_case_name_is_not);
    }
}

INTERNAL_FUNCTION void test_index(BUTTestCase *tc) {
    TestDriverData *tdd   = CONTAINER_OF(tc, TestDriverData, tc);
    size_t          index = tdd->get_index(&tdd->context);

    if (index != 0) {
        EXM_THROW(unexpected_index);
    }

    tdd->next(&tdd->context);
    if (tdd->get_index(&tdd->context) != 1) {
        EXM_THROW(unexpected_index);
    }
}

INTERNAL_FUNCTION void test_test(BUTTestCase *tc) {
    TestDriverData *tdd = CONTAINER_OF(tc, TestDriverData, tc);

    EXM_TRY {
        tdd->test(&tdd->context,
                  tdd->context.env.bts->test_cases[tdd->context.env.index]);
        tdd->next(&tdd->context);
        tdd->test(&tdd->context,
                  tdd->context.env.bts->test_cases[tdd->context.env.index]);
        // Should have thrown failure to be caught in the EXM_CATCH block below
        EXM_THROW(test_throw_failure);
    }
    EXM_CATCH(exm_expected_failure) {
        // expected failure
        tdd->context.env.test_failures++;
        tdd->context.env.results_count++;
        tdd->context.env.run_count++;
    }
    EXM_CATCH_ALL {
        // If anything else was caught it's a failure. Rethrow the exception.
        EXM_RETHROW;
    }
    EXM_END_TRY;

    size_t pass = tdd->get_pass_count(&tdd->context);
    if (pass != 1) {
        EXM_THROW(expected_one_passing_test);
    }

    if (tdd->get_fail_count(&tdd->context) != 1) {
        EXM_THROW(expected_one_failing_test);
    }

    if (tdd->get_failed_setup_count(&tdd->context) != 0) {
        EXM_THROW(unexpected_setup_failure_count);
    }

    if (tdd->get_results_count(&tdd->context) != 1) {
        EXM_THROW(unexpected_test_result_count);
    }
}

INTERNAL_FUNCTION void test_result(BUTTestCase *tc) {
    TestDriverData *tdd = CONTAINER_OF(tc, TestDriverData, tc);

    tdd->test(&tdd->context, tdd->context.env.bts->test_cases[tdd->context.env.index]);
    tdd->next(&tdd->context);
    EXM_TRY {
        tdd->test(&tdd->context,
                  tdd->context.env.bts->test_cases[tdd->context.env.index]);
        // Should have failed
        EXM_THROW(exm_expected_failure);
    }
    EXM_CATCH(exm_expected_failure) {
        ; // Expected failure
    }
    EXM_END_TRY;
}
