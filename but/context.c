/**
 * @file context.c
 * @author Douglas Cuthbertson
 * @brief Manage context information for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <but/context.h>           // BUTContext, ResultContext, BUTResultCode
#include <but/but.h>               // BUTTestCase, BUTTestSuite
#include <exceptions/exceptions.h> // EXM_TRY, EXM_END_TRY, etc.
#include <exceptions/types.h>      // EXMReason
#include <platform.h>              // u32
#include <win32/intrinsics.h>      // intrinsic memset

#include <stddef.h> // NULL
#include <stdlib.h> // realloc
#include <memory.h> // memset

static EXMReason invalid_test_case = "invalid test case";

/**
 * @brief The status and exception generated from a test case that did not pass.
 *
 * The ResultContext only exists for tests that ran, but didn't pass. All tests that run
 * and pass don't generate one of these.
 */
struct ResultContext {
    u32           index;  ///< the index of the test case
    BUTResultCode status; ///< indicate state of test result
    char const   *reason; ///< a string describing the reason for a failure
};

/**
 * @brief increase the capacity of the results array in the context's environment.
 *
 * @param ctx
 */
static void grow_capacity(BUTContext *ctx) {
    ResultContext *new_results;
    u32            new_capacity, count;
    u32 const      increment = 10;

    // Calculate a new capacity up to the number of test cases in the test suite.
    if (ctx->env.results_capacity + increment > ctx->env.test_case_count) {
        new_capacity = ctx->env.test_case_count;
        count        = ctx->env.test_case_count - ctx->env.results_capacity;
    } else {
        new_capacity = ctx->env.results_capacity + increment;
        count        = increment;
    }

    new_results = realloc(ctx->env.results, new_capacity * sizeof(ResultContext));
    if (new_results) {
        // Initialize new memory block
        memset(&new_results[ctx->env.results_capacity], 0,
               count * sizeof(ResultContext));
        ctx->env.results_capacity = new_capacity;
        ctx->env.results          = new_results;
    }
}

/**
 * @brief new_result creates a new result context to capture the reason for a test
 * failure.
 *
 * @param ctx a test context.
 * @param status a test-result code.
 * @param reason a string describing the reason for the test failure.
 */
static void new_result(BUTContext *ctx, BUTResultCode status, char const *reason) {
    if (ctx->env.results_count == ctx->env.results_capacity) {
        grow_capacity(ctx);
    }

    if (ctx->env.results_count < ctx->env.results_capacity) {
        ctx->env.results[ctx->env.results_count].index  = ctx->env.index;
        ctx->env.results[ctx->env.results_count].status = status;
        ctx->env.results[ctx->env.results_count].reason = reason;
        ctx->env.results_count++;
    }
}

// Check the validity of the test context
bool but_is_valid(BUTContext *ctx) {
    bool valid = false;

    if (ctx) {
        if (ctx->env.magic == (uintptr_t)ctx
            && ctx->env.index <= ctx->env.test_case_count) {
            valid = true;
        }
    }

    return valid;
}

void but_initialize(BUTContext *ctx) {
    memset(ctx, 0, sizeof(BUTContext));
    ctx->env.magic = (uintptr_t)ctx;
}

void but_begin(BUTContext *ctx, struct BUTTestSuite *bts) {
    ctx->env.bts             = bts;
    ctx->env.test_case_count = bts->count;
}

void but_end(BUTContext *ctx) {
    if (ctx->env.results) {
        free(ctx->env.results);
        ctx->env.results = 0;
    }
}

void but_next(BUTContext *ctx) {
    if (ctx->env.index < ctx->env.test_case_count) {
        ctx->env.index++;
    }
}

// Return true if there are more test cases
bool but_has_more(BUTContext *ctx) {
    return ctx->env.index < ctx->env.test_case_count;
}

char const *but_get_test_case_name(BUTContext *ctx) {
    char const   *name;
    BUTTestSuite *bts = ctx->env.bts;

    if (ctx->env.index >= 0 && ctx->env.index < ctx->env.test_case_count) {
        name = bts->test_cases[ctx->env.index]->name;
    } else {
        name = "test case index out of range";
    }

    return name;
}

u32 but_get_index(BUTContext *ctx) {
    return ctx->env.index;
}

void but_test(BUTContext *ctx) {
    BUTResultCode result = BUT_PASSED;
    BUTTestCase  *tc     = ctx->env.bts->test_cases[ctx->env.index];

    if (tc == NULL) {
        result = BUT_FAILED;
        new_result(ctx, BUT_FAILED, invalid_test_case);
        ctx->env.test_failures++;
        EXM_THROW_DETAILS(invalid_test_case, "test case %u does not exist",
                          ctx->env.index);
    }

    if (tc->setup != NULL) {
        EXM_TRY {
            tc->setup(tc);
        }
        EXM_CATCH_ALL {
            if (BUT_UNEXPECTED_EXCEPTION(EXM_REASON)) {
                result = BUT_FAILED_SETUP;
                new_result(ctx, BUT_FAILED_SETUP, EXM_REASON);
                ctx->env.setup_failures++;
            }
            ctx->env.run_count++;
            EXM_RETHROW;
        }
        EXM_END_TRY;
    }

    if (result == BUT_PASSED) {
        if (tc->test != NULL) {
            EXM_TRY {
                tc->test(tc);
            }
            EXM_CATCH_ALL {
                if (BUT_UNEXPECTED_EXCEPTION(EXM_REASON)) {
                    new_result(ctx, BUT_FAILED, EXM_REASON);
                    ctx->env.test_failures++;
                }
                ctx->env.run_count++;
                EXM_RETHROW;
            }
            EXM_END_TRY;
        }
        ctx->env.run_count++;
    }

    if (tc->cleanup != NULL) {
        EXM_TRY {
            tc->cleanup(tc);
        }
        EXM_CATCH_ALL {
            if (BUT_UNEXPECTED_EXCEPTION(EXM_REASON)) {
                new_result(ctx, BUT_FAILED_CLEANUP, EXM_REASON);
                ctx->env.cleanup_failures++;
            }
            EXM_RETHROW;
        }
        EXM_END_TRY;
    }
}

u32 but_get_run_count(BUTContext *ctx) {
    return ctx->env.run_count;
}

u32 but_get_pass_count(BUTContext *ctx) {
    return ctx->env.test_case_count
           - (ctx->env.test_failures + ctx->env.setup_failures
              + ctx->env.cleanup_failures);
}

u32 but_get_test_failure_count(BUTContext *ctx) {
    return ctx->env.test_failures;
}

u32 but_get_setup_failure_count(BUTContext *ctx) {
    return ctx->env.setup_failures;
}

u32 but_get_cleanup_failure_count(BUTContext *ctx) {
    return ctx->env.cleanup_failures;
}

u32 but_get_results_count(BUTContext *ctx) {
    return ctx->env.results_count;
}

/*
 * Get the pass/fail results of running the test case that maps to the given
 * index.
 */
BUTResultCode but_get_result(BUTContext *ctx, u32 index) {
    BUTResultCode but_result;
    u32           i;

    if (ctx->env.results_count == 0) {
        // No results were generated, so all tests passed
        but_result = BUT_PASSED;
    } else {
        // Search the results and see if there's one matching the index
        i = 0;
        while (i < ctx->env.results_count && ctx->env.results[i].index < index) {
            i++;
        }

        if (i < ctx->env.results_count && ctx->env.results[i].index == index) {
            // We have a match
            but_result = ctx->env.results[i].status;
        } else {
            // There's no result, so if it ran it passed
            but_result = BUT_PASSED;
        }
    }

    return but_result;
}
