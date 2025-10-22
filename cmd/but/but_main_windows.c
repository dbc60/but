/**
 * @file but_main_windows.c
 * @author Douglas Cuthbertson
 * @brief The test driver for the Basic Unit Test (BUT) library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "../../src/but_driver.c"
#include "../../src/but_result_context.c"
#include "../../src/exception_assert.c"
#include "../../src/exception.c"
#include "../../src/log.c"

#include <but_macros.h> // BUT_CONTAINER

#include <stddef.h> // size_t
#include <stdio.h>  // printf, snprintf
#include <stdlib.h> // exit

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h> // HMODULE

/**
 * @brief The exception handler for the BUT test driver.
 *
 * It's basically the same as the default exception handler, but it prints its output to
 * stdout instead of stderr.
 *
 * @param ctx a pointer to the current BUTExceptionContext.
 * @param reason a brief reason for throwing the exception.
 * @param details a (possibly NULL) string providing more details about the exception.
 * @param file the path to the file in which the exception was thrown.
 * @param line the line number on which the exception was thrown.
 */
static BUT_HANDLER_FN(exception_handler) {
    BUTContext *bctx = BUT_CONTAINER(ctx, BUTContext, exception_context);

    if (BUT_UNEXPECTED_EXCEPTION(reason)) {
        BUTTestSuite *bts = bctx->env.bts;
        char         *name;
        if (bctx->env.index < bts->count) {
            name = bts->test_cases[bctx->env.index]->name;
        } else {
            name = "Unknown";
        }
        but_log_error(name, reason, details, file, line);
    }
}

static void display_test_case(BUTContext *bctx) {
    char        counter_buf[16]; // 16 bytes should be plenty for a counter.
    char const *test_case_name;
    size_t      idx;

    test_case_name = but_get_test_case_name(bctx);
    idx            = but_get_index(bctx);

    snprintf(counter_buf, sizeof counter_buf, "%6zu. ", idx + 1);
    // Display  "(leading text) TestCaseName (end text)"
    printf("%s%s\n", counter_buf, test_case_name);
}

static void display_test_results(BUTContext *bctx, BUTTestSuite *bts) {
    size_t passed, setup_failures, test_failures, cleanup_failures, count_total_failures;
    char   counter_buf[6]  = {0};
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
        counter_buf[i] = ' ';
    }

    passed           = but_get_pass_count(bctx);
    size_t run_count = but_get_run_count(bctx);
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

    setup_failures       = but_get_setup_failure_count(bctx);
    test_failures        = but_get_test_failure_count(bctx);
    cleanup_failures     = but_get_cleanup_failure_count(bctx);
    count_total_failures = setup_failures + test_failures + cleanup_failures;

    if (count_total_failures > 0) {
        printf("Failures: %zu\n", count_total_failures);
        printf("%sFailed Setups: %zu\n", counter_buf, setup_failures);
        printf("%sFailed Tests: %zu\n", counter_buf, test_failures);
        printf("%sFailed Cleanups: %zu\n", counter_buf, cleanup_failures);
    }
}

static void exercise_test_suite(BUTContext *bctx, BUTTestSuite *bts) {
    but_begin(bctx, bts);
    while (but_has_more(bctx)) {
        display_test_case(bctx);
        BUT_TRY {
            but_driver(bctx);
        }
        BUT_CATCH_ALL {
            BUT_RETHROW;
        }
        BUT_END_TRY;
        but_next(bctx);
    }

    display_test_results(bctx, bts);
}

/**
 * @brief the entry point for a simple command-line test driver.
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv) {
    int                i;
    char              *ts_path;
    HMODULE            test_suite;
    but_get_test_suite get_test_suite;
    BUTTestSuite      *bts;
    BUTContext         bctx;

    if (argc > 1) {
        int test_suites = 0;
        logger_init();
        logger_set_level(LOG_INFO);
        logger_set_output_by_filename("but.log");

        BUT_TRY {
            // Assume each argument is a path to a test suite
            for (i = 1; i < argc; i++) {
                ts_path    = argv[i];
                test_suite = LoadLibraryA(ts_path);
                if (test_suite) {
                    but_set_exception_context_fn *set_context
                        = (but_set_exception_context_fn *)
                            GetProcAddress(test_suite, "but_set_exception_context");
                    if (set_context == NULL) {
                        // ensure the pointer is not null
                        set_context = but_set_exception_context;
                        printf("Error: test suite %s doesn't export "
                               "but_set_exception_context\n",
                               argv[i]);
                    }

                    get_test_suite
                        = (but_get_test_suite)GetProcAddress(test_suite,
                                                             "get_test_suite");
                    if (get_test_suite) {
                        but_initialize(&bctx, exception_handler);
                        // register our exception handler with the test suite.
                        set_context(&bctx.exception_context, __FILE__, __LINE__);
                        bts = get_test_suite();
                        printf("\n%s (%u): test suite %d of %d\n", bts->name, bts->count,
                               i, argc - 1);
                        exercise_test_suite(&bctx, bts);
                        test_suites++;
                        if (i < argc) {
                            printf("*******************************************\n");
                        }
                    } else {
                        printf("Error: test suite %s doesn't export get_test_suite\n",
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
        }
        BUT_FINALLY {
            logger_close();
        }
        BUT_END_TRY;
    } else {
        printf("Usage: %s (path to test suite)+\n", argv[0]);
    }

    return 0;
}
