/**
 * @file but.c
 * @author Douglas Cuthbertson
 * @brief The test driver for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <but/context.h>           // BUTContext
#include <but/but.h>               // BUT_UNEXPECTED_EXCEPTION
#include <exceptions/types.h>      // exm_closure, EXMReason, exm_register_closure_fn
#include <exceptions/exceptions.h> // exm_register_closure
#include <project.h>               // CONTAINER_OF

#include <stddef.h> // size_t
#include <stdio.h>  // printf
#include <stdlib.h> // exit

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h> // HMODULE

static EXMReason continue_to_next_test = "continue to next test";

/**
 * @brief The exception handler for the BUT test driver.
 *
 * It's basically the same as the default exception handler, but it prints its output to
 * stdout instead of stderr.
 *
 * @param closure a closure within an environment.
 * @param reason a brief reason for throwing the exception.
 * @param details a (possibly NULL) string providing more details about the exception.
 * @param file the path to the file in which the exception was thrown.
 * @param line the line number on which the exception was thrown.
 */
static void but_exception_handler(exm_closure *closure, EXMReason reason,
                                  char const *details, char const *file, int line) {
    UNUSED_PARAMETER(closure);

    if (BUT_UNEXPECTED_EXCEPTION(reason)) {
        if (details == NULL) {
            printf("        %s @%s:%d\n", reason, file, line);
        } else {
            printf("        %s: %s @%s:%d\n", reason, details, file, line);
        }
        EXM_THROW(continue_to_next_test);
    }
}

static void display_test_case(BUTContext *ctx) {
    char        buf[16]; // 16 bytes should be plenty for a counter.
    char const *test_case_name;
    size_t      idx;

    test_case_name = but_get_test_case_name(ctx);
    idx            = but_get_index(ctx);

    snprintf(buf, sizeof(buf), "%6zu. ", idx + 1);
    // Display  "(leading text) TestCaseName (end text)"
    printf("%s%s\n", buf, test_case_name);
}

static void display_test_results(BUTContext *ctx, BUTTestSuite *bts) {
    size_t passed, setup_failures, test_failures, cleanup_failures, count_total_failures;
    char   buf[6]          = {0};
    size_t test_case_count = bts->count;
    int    spaces          = 5;
    int    magnitude       = (int)test_case_count;

    while (spaces > 0 && magnitude > 0) {
        magnitude /= 10;
        if (magnitude > 0) {
            spaces--;
        }
    }

    for (int i = 0; i < spaces; i++) {
        buf[i] = ' ';
    }

    passed           = but_get_pass_count(ctx);
    size_t run_count = but_get_run_count(ctx);
    if (passed != run_count) {
        printf("\nPassed: %zu of %zu test cases\n", passed, run_count);
    } else {
        if (passed == 2) {
            puts("\nBoth tests passed");
        } else if (passed == 1) {
            puts("\nThe test passed");
        } else {
            printf("\nAll %zu tests passed\n", passed);
        }
    }

    setup_failures       = but_get_setup_failure_count(ctx);
    test_failures        = but_get_test_failure_count(ctx);
    cleanup_failures     = but_get_cleanup_failure_count(ctx);
    count_total_failures = setup_failures + test_failures + cleanup_failures;

    if (count_total_failures > 0) {
        printf("Failures: %zu\n", count_total_failures);
        printf("%sFailed Setups: %zu\n", buf, setup_failures);
        printf("%sFailed Tests: %zu\n", buf, test_failures);
        printf("%sFailed Cleanups: %zu\n", buf, cleanup_failures);
    }
}

static void exercise_test_suite(BUTContext *ctx, BUTTestSuite *bts) {
    but_begin(ctx, bts);
    while (but_has_more(ctx)) {
        display_test_case(ctx);
        EXM_TRY {
            but_test(ctx);
        }
        EXM_CATCH(continue_to_next_test) {
            ; // continue
        }
        EXM_END_TRY;
        but_next(ctx);
    }

    display_test_results(ctx, bts);
    if (ctx->env.results_count > 0) {
        printf("\nExiting with %u test failures.\n", ctx->env.results_count);
        exit(-1);
    }
}

// The command-line test driver interface
int main(int argc, char **argv) {
    int                i;
    char              *ts_path;
    HMODULE            test_suite;
    but_get_test_suite get_test_suite;
    BUTTestSuite      *bts;
    BUTContext         ctx;

    if (argc > 1) {
        int test_suites = 0;

        // Assume each argument is a path to a test suite
        for (i = 1; i < argc; i++) {
            ts_path    = argv[i];
            test_suite = LoadLibraryA(ts_path);
            if (test_suite) {
                exm_register_closure_fn register_closure
                    = (exm_register_closure_fn)GetProcAddress(test_suite,
                                                              "exm_register_closure");
                if (register_closure == NULL) {
                    // ensure the pointer is not null
                    register_closure = exm_register_closure;
                    printf("Error: test suite %s doesn't export exm_register_closure\n",
                           argv[i]);
                }

                get_test_suite
                    = (but_get_test_suite)GetProcAddress(test_suite, "but_test_suite");
                if (get_test_suite) {
                    but_initialize(&ctx);
                    bts                 = get_test_suite();
                    ctx.exm_ctx.handler = but_exception_handler;
                    // register our exception handler with both the local exception
                    // manager and the test suite's exception manager
                    exm_register_closure(&ctx.exm_ctx.handler);
                    register_closure(&ctx.exm_ctx.handler);
                    printf("\n%s (%u): test suite %d of %d\n", bts->name, bts->count, i,
                           argc - 1);
                    exercise_test_suite(&ctx, bts);
                    test_suites++;
                    if (i < argc) {
                        printf("*******************************************\n");
                    }
                } else {
                    printf("Error: test suite %s doesn't export but_test_suite\n",
                           argv[i]);
                }

                FreeLibrary(test_suite);
            } else {
                printf("Failed to load test suite %s, error = %lu\n", ts_path,
                       GetLastError());
            }
        }
        if (argc == 2) {
            printf("\nExercised 1 test suite.\n");
        } else {
            printf("\nExercised %d of %d test suites.\n", test_suites, argc - 1);
        }
    } else {
        printf("Usage: %s <path to test suite>+\n", argv[0]);
    }

    return 0;
}
