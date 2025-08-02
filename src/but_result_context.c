/**
 * @file but_result_context.c
 * @author Douglas Cuthbertson
 * @brief
 * @version 0.1
 * @date 2025-05-08
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 *
 */
#include "but_context.h"        // BUTContext
#include "but_result_context.h" // ResultContext
#include "intrinsics_win32.h"

#include <abbreviated_types.h> // u32

#include <stddef.h> // NULL
#include <stdlib.h> // realloc
#include <string.h> // memset

/**
 * @brief increase the capacity of the results array in the context's environment.
 *
 * @param bctx
 */
static void grow_capacity(BUTContext *bctx) {
    ResultContext *new_results;
    u32            new_capacity, count;
    u32 const      increment = 10;

    // Calculate a new capacity up to the number of test cases in the test suite.
    if (bctx->env.results_capacity + increment > bctx->env.test_case_count) {
        new_capacity = bctx->env.test_case_count;
        count        = bctx->env.test_case_count - bctx->env.results_capacity;
    } else {
        new_capacity = bctx->env.results_capacity + increment;
        count        = increment;
    }

    new_results = realloc(bctx->env.results, new_capacity * sizeof(ResultContext));
    if (new_results) {
        // Initialize new memory block
        memset(&new_results[bctx->env.results_capacity], 0, count * sizeof(ResultContext));
        bctx->env.results_capacity = new_capacity;
        bctx->env.results          = new_results;
    }
}

// create a new result context to capture the reason for a test failure.
void new_result(BUTContext *bctx, BUTResultCode status, char const *reason) {
    if (bctx->env.results_count == bctx->env.results_capacity) {
        grow_capacity(bctx);
    }

    if (bctx->env.results_count < bctx->env.results_capacity) {
        bctx->env.results[bctx->env.results_count].index  = bctx->env.index;
        bctx->env.results[bctx->env.results_count].status = status;
        bctx->env.results[bctx->env.results_count].reason = reason;
        bctx->env.results_count++;
    }
}
