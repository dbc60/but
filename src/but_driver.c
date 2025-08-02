/**
 * @file but_driver.c
 * @author Douglas Cuthbertson
 * @brief Manage context information for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "but_driver.h"
#include "but_result_context.h" // new_result
#include "intrinsics_win32.h"

#include <exception_types.h> // BUTExceptionReason

#include <stdbool.h> // bool, true, false
#include <stdint.h>  // uintptr_t
#include <stdlib.h>  // free

static BUTExceptionReason invalid_test_case = "invalid test case";

// Check the validity of the test context
BUT_IS_VALID(but_is_valid) {
    bool valid = false;

    if (bctx) {
        if (bctx->env.initialized && bctx->env.index <= bctx->env.test_case_count) {
            valid = true;
        }
    }

    return valid;
}

// initialize the test context
BUT_INITIALIZE(but_initialize) {
    memset(bctx, 0, sizeof bctx);
    memset(&bctx->env, 0, sizeof bctx->env);
    if (handler != NULL) {
        bctx->but_ctx.handler = handler;
        but_set_exception_context(&bctx->but_ctx);
    } else {
        bctx->but_ctx.handler = but_default_handler;
    }
    bctx->but_ctx.stack   = NULL; // no exception stack yet
    bctx->env.initialized = true;
}

// assign a test suite to a test context
BUT_BEGIN(but_begin) {
    bctx->env.bts             = bts;
    bctx->env.test_case_count = bts->count;
}

// release the memory resources allocated during testing
BUT_END(but_end) {
    if (bctx->env.results) {
        free(bctx->env.results);
        bctx->env.results = 0;
    }
}

// Move to the next test case
BUT_NEXT(but_next) {
    if (bctx->env.index < bctx->env.test_case_count) {
        bctx->env.index++;
    }
}

// Return true if there are more test cases
BUT_HAS_MORE(but_has_more) {
    return bctx->env.index < bctx->env.test_case_count;
}

// Get the name of the current test case
BUT_GET_TEST_CASE_NAME(but_get_test_case_name) {
    char const   *name;
    BUTTestSuite *bts = bctx->env.bts;

    if (bctx->env.index >= 0 && bctx->env.index < bctx->env.test_case_count) {
        name = bts->test_cases[bctx->env.index]->name;
    } else {
        name = "test case index out of range";
    }

    return name;
}

// Get the index of the current test case
BUT_GET_INDEX(but_get_index) {
    return bctx->env.index;
}

// Execute the current test case
BUT_DRIVER(but_driver) {
    BUTResultCode result = BUT_PASSED;
    BUTTestCase  *tc     = bctx->env.bts->test_cases[bctx->env.index];

    if (tc == NULL) {
        result = BUT_FAILED;
        new_result(bctx, BUT_FAILED, invalid_test_case);
        bctx->env.test_failures++;
        BUT_THROW_DETAILS(invalid_test_case, "test case %u does not exist", bctx->env.index);
    }

    if (tc->setup != NULL) {
        BUT_TRY {
            tc->setup(tc);
        }
        BUT_CATCH_ALL {
            if (BUT_UNEXPECTED_EXCEPTION(BUT_REASON)) {
                result = BUT_FAILED_SETUP;
                new_result(bctx, BUT_FAILED_SETUP, BUT_REASON);
                bctx->env.setup_failures++;
            }
            bctx->env.run_count++;
            BUT_RETHROW;
        }
        BUT_END_TRY;
    }

    if (result == BUT_PASSED) {
        if (tc->test != NULL) {
            BUT_TRY {
                tc->test(tc);
            }
            BUT_CATCH_ALL {
                if (BUT_UNEXPECTED_EXCEPTION(BUT_REASON)) {
                    new_result(bctx, BUT_FAILED, BUT_REASON);
                    bctx->env.test_failures++;
                }
                bctx->env.run_count++;
                BUT_RETHROW;
            }
            BUT_END_TRY;
        }
        bctx->env.run_count++;
    }

    if (tc->cleanup != NULL) {
        BUT_TRY {
            tc->cleanup(tc);
        }
        BUT_CATCH_ALL {
            if (BUT_UNEXPECTED_EXCEPTION(BUT_REASON)) {
                new_result(bctx, BUT_FAILED_CLEANUP, BUT_REASON);
                bctx->env.cleanup_failures++;
            }
            BUT_RETHROW;
        }
        BUT_END_TRY;
    }
}

// Get the number of test cases executed
BUT_GET_RUN_COUNT(but_get_run_count) {
    return bctx->env.run_count;
}

// Get the number of test cases that passed
BUT_GET_PASS_COUNT(but_get_pass_count) {
    return bctx->env.test_case_count - (bctx->env.test_failures + bctx->env.setup_failures + bctx->env.cleanup_failures);
}

// Get the number of test cases that failed
BUT_GET_TEST_FAILURE_COUNT(but_get_test_failure_count) {
    return bctx->env.test_failures;
}

// Get the number of test cases where setup failed
BUT_GET_SETUP_FAILURE_COUNT(but_get_setup_failure_count) {
    return bctx->env.setup_failures;
}

// Get the number of test cases that failed in the cleanup phase
BUT_GET_CLEANUP_FAILURE_COUNT(but_get_cleanup_failure_count) {
    return bctx->env.cleanup_failures;
}

// Get the number of result contexts
BUT_GET_RESULTS_COUNT(but_get_results_count) {
    return bctx->env.results_count;
}

// Get the result code from the result context of a test case
BUT_GET_RESULT(but_get_result) {
    BUTResultCode but_result;
    u32           i;

    if (bctx->env.results_count == 0) {
        // No results were generated, so all tests passed
        but_result = BUT_PASSED;
    } else {
        // Search the results and see if there's one matching the index
        i = 0;
        while (i < bctx->env.results_count && bctx->env.results[i].index < index) {
            i++;
        }

        if (i < bctx->env.results_count && bctx->env.results[i].index == index) {
            // We have a match
            but_result = bctx->env.results[i].status;
        } else {
            // There's no result, so if it ran it passed
            but_result = BUT_PASSED;
        }
    }

    return but_result;
}
