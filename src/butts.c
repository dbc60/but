/**
 * @file butts.c
 * @author Douglas Cuthbertson
 * @brief A test suite for the exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <but/but.h>               // BUTTestCase, BUTTestSuite
#include <but/context.h>           // BUTContext
#include <exceptions/assert.h>     // EXM_ASSERT
#include <exceptions/types.h>      // EXMReason
#include <exceptions/exceptions.h> // EXM_THROW, EXM_TRY, etc.
#include <project.h>               // DATA_SPEC_EXPORT

#include <stddef.h> // NULL

static EXMReason test_exception = "test exception";

INTERNAL_FUNCTION void test_throw(BUTTestCase *btc) {
    UNUSED_PARAMETER(btc);
    EXM_TRY {
        EXM_THROW(test_exception);
    }
    EXM_CATCH(test_exception) {
        ; // Success
    }
    EXM_END_TRY;
}

INTERNAL_FUNCTION void test_throw_file_line(BUTTestCase *btc) {
    UNUSED_PARAMETER(btc);
    EXM_TRY {
        EXM_THROW_FILE_LINE(test_exception, __FILE__, __LINE__);
    }
    EXM_CATCH(test_exception) {
        ; // Success
    }
    EXM_END_TRY;
}

INTERNAL_FUNCTION void test_throw_details(BUTTestCase *btc) {
    UNUSED_PARAMETER(btc);
    EXM_TRY {
        EXM_THROW_DETAILS(test_exception, "throw details test");
    }
    EXM_CATCH(test_exception) {
        ; // Success
    }
    EXM_END_TRY;
}

INTERNAL_FUNCTION void test_throw_details_file_line(BUTTestCase *btc) {
    UNUSED_PARAMETER(btc);
    EXM_TRY {
        EXM_THROW_DETAILS_FILE_LINE(test_exception,
                                    "test throw details with file and line number",
                                    __FILE__, __LINE__);
    }
    EXM_CATCH(test_exception) {
        ; // Success
    }
    EXM_END_TRY;
}

INTERNAL_FUNCTION void test_throw_var_args(BUTTestCase *btc) {
    UNUSED_PARAMETER(btc);
    EXM_TRY {
        EXM_THROW_DETAILS(test_exception, "test");
    }
    EXM_CATCH(test_exception) {
        ; // Success
    }
    EXM_END_TRY;
}

INTERNAL_FUNCTION void test_throw_var_args_file_line(BUTTestCase *btc) {
    UNUSED_PARAMETER(btc);
    EXM_TRY {
        EXM_THROW_DETAILS_FILE_LINE(test_exception, "test", "source file", 0);
    }
    EXM_CATCH(test_exception) {
        ; // Success
    }
    EXM_END_TRY;
}

INTERNAL_FUNCTION void test_throw_rethrow(BUTTestCase *btc) {
    UNUSED_PARAMETER(btc);
    EXM_TRY {
        EXM_TRY {
            EXM_THROW(test_exception);
        }
        EXM_CATCH_ALL {
            EXM_RETHROW;
        }
        EXM_END_TRY;
    }
    EXM_CATCH(test_exception) {
        ; // Success
    }
    EXM_END_TRY;
}

INTERNAL_FUNCTION void test_throw_catch_all(BUTTestCase *btc) {
    UNUSED_PARAMETER(btc);
    EXM_TRY {
        EXM_TRY {
            EXM_THROW(test_exception);
        }
        EXM_CATCH(exm_invalid_value) {
            // should not catch here
            EXM_ASSERT(0);
        }
        EXM_END_TRY;
    }
    EXM_CATCH_ALL {
        ; // should catch here
    }
    EXM_END_TRY;
}

/**
 * @brief throw an explicit exception that will cause the test driver to report
 * an uncaught exception.
 *
 * @param btc
 */
INTERNAL_FUNCTION void test_failure(BUTTestCase *btc) {
    UNUSED_PARAMETER(btc);
    EXM_THROW(exm_expected_failure);
}

static BUTTestCase test_exm_throw = {"Throw", NULL, test_throw, NULL};
static BUTTestCase test_exm_throw_file_line
    = {"Throw with File and Line Number", NULL, test_throw_file_line, NULL};
static BUTTestCase test_exm_throw_details
    = {"Throw with Details", NULL, test_throw_details, NULL};
static BUTTestCase test_exm_throw_details_file_line
    = {"Throw with Details, File, and Line Number", NULL, test_throw_details_file_line,
       NULL};
static BUTTestCase test_exm_throw_va
    = {"Throw with Var Args", NULL, test_throw_var_args, NULL};
static BUTTestCase test_exm_throw_va_file_line
    = {"Throw with Var Args, File, and Line Number", NULL, test_throw_var_args_file_line,
       NULL};
static BUTTestCase test_exm_throw_rethrow
    = {"Throw and Rethrow", NULL, test_throw_rethrow, NULL};
static BUTTestCase test_exm_throw_catch_all
    = {"Throw and Catch All", NULL, test_throw_catch_all, NULL};
static BUTTestCase test_exm_test_failure
    = {"Expected Test Failure", NULL, test_failure, NULL};
static BUTTestCase test_exm_setup_failure
    = {"Expected Test Setup Failure", test_failure, NULL, NULL};
static BUTTestCase test_exm_cleanup_failure
    = {"Expected Test Cleanup Failure", NULL, NULL, test_failure};

static BUTTestCase *tca[] = {&test_exm_throw,          &test_exm_throw_file_line,
                             &test_exm_throw_details,  &test_exm_throw_details_file_line,
                             &test_exm_throw_va,       &test_exm_throw_va_file_line,
                             &test_exm_throw_rethrow,  &test_exm_throw_catch_all,
                             &test_exm_test_failure,   &test_exm_setup_failure,
                             &test_exm_cleanup_failure};

static BUTTestSuite test_suite = {"Exceptions", sizeof tca / sizeof(BUTTestCase *), tca};

DATA_SPEC_EXPORT BUTTestSuite *but_test_suite(void) {
    return &test_suite;
}
