/**
 * @file butts.c
 * @author Douglas Cuthbertson
 * @brief A test suite for the exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "exception.c"
#include "exception_assert.c"

#include <but.h>              // BUTTestCase, BUTTestSuite
#include <exception_assert.h> // assert
#include <exception_types.h>  // BUTExceptionReason
#include <exception.h>        // BUT_THROW, BUT_TRY, etc.
#include <but_macros.h>       // DLL_SPEC_EXPORT, BUT_UNUSED

#include <stddef.h> // NULL

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

static BUTExceptionReason test_exception = "test exception";

BUT_TEST("Throw", test_throw) {
    BUT_TRY {
        BUT_THROW(test_exception);
    }
    BUT_CATCH(test_exception) {
        ; // Success
    }
    BUT_END_TRY;
}

BUT_TEST("Throw File Line", test_throw_file_line) {
    BUT_TRY {
        BUT_THROW_FILE_LINE(test_exception, __FILE__, __LINE__);
    }
    BUT_CATCH(test_exception) {
        ; // Success
    }
    BUT_END_TRY;
}

BUT_TEST("Throw Details", test_throw_details) {
    BUT_TRY {
        BUT_THROW_DETAILS(test_exception, "throw details test");
    }
    BUT_CATCH(test_exception) {
        ; // Success
    }
    BUT_END_TRY;
}

BUT_TEST("Throw Details File Line", test_throw_details_file_line) {
    BUT_TRY {
        BUT_THROW_DETAILS_FILE_LINE(test_exception, "test throw details with file and line number", __FILE__, __LINE__);
    }
    BUT_CATCH(test_exception) {
        ; // Success
    }
    BUT_END_TRY;
}

BUT_TEST("Throw Var Args", test_throw_va) {
    BUT_TRY {
        BUT_THROW_DETAILS(test_exception, "test");
    }
    BUT_CATCH(test_exception) {
        ; // Success
    }
    BUT_END_TRY;
}

BUT_TEST("Throw Var Args File Line", test_throw_va_file_line) {
    BUT_TRY {
        BUT_THROW_DETAILS_FILE_LINE(test_exception, "test", "source file", 0);
    }
    BUT_CATCH(test_exception) {
        ; // Success
    }
    BUT_END_TRY;
}

BUT_TEST("Throw and Rethrow", test_throw_rethrow) {
    BUT_TRY {
        BUT_TRY {
            BUT_THROW(test_exception);
        }
        BUT_CATCH_ALL {
            BUT_RETHROW;
        }
        BUT_END_TRY;
    }
    BUT_CATCH(test_exception) {
        ; // Success
    }
    BUT_END_TRY;
}

BUT_TEST("Throw and Catch All", test_throw_catch_all) {
    BUT_TRY {
        BUT_TRY {
            BUT_THROW(test_exception);
        }
        BUT_CATCH(but_invalid_value) {
            // should not catch here
            assert(0);
        }
        BUT_END_TRY;
    }
    BUT_CATCH_ALL {
        ; // should catch here
    }
    BUT_END_TRY;
}

/**
 * @brief throw an explicit exception that will be caught by the test driver.
 *
 * @param btc
 */
BUT_TEST_FN(test_failure) {
    BUT_UNUSED(btc);
    BUT_THROW(but_expected_failure);
}

BUT_CASE("Expected Test Failure", test_failure, NULL, NULL);
BUT_CASE_NAME("Expected Test Setup Failure", setup_failure, NULL, test_failure, NULL);
BUT_CASE_NAME("Expected Test Cleanup Failure", cleanup_failure, NULL, NULL, test_failure);

/**
 * @brief Throw but_internal_error from multiple nested contexts to ensure it is caught correctly.
 */
BUT_TEST("Deep Nesting", deep_nesting) {
    BUT_TRY {
        BUT_TRY {
            BUT_TRY {
                BUT_TRY {
                    BUT_THROW(but_test_exception);
                }
                BUT_END_TRY;
            }
            BUT_END_TRY;
        }
        BUT_END_TRY;
    }
    BUT_CATCH(but_test_exception) {
        ; // Success - exception propagated through all levels
    }
    BUT_END_TRY;
}

static void call3() {
    BUT_THROW(but_test_exception);
}

static void call2() {
    call3();
}

static void call1() {
    call2();
}

static void call0() {
    call1();
}

/**
 * @brief This test is designed to ensure that the exception handling works correctly
 * even when the call stack is deep.
 */
BUT_TEST("Deep Call Stack", deep_call_stack) {
    // This test is designed to ensure that the exception handling works correctly
    // even when the call stack is deep.
    BUT_TRY {
        call0();
    }
    BUT_CATCH(but_test_exception) {
        ; // Success - exception caught
    }
    BUT_END_TRY;
}

BUT_TEST("Finally Block", finally_block) {
    int finally_called = 0;
    int catch_called   = 0;

    BUT_TRY {
        BUT_THROW(but_test_exception);
    }
    BUT_CATCH(but_test_exception) {
        ; // Expected exception
        catch_called = 1;
    }
    BUT_FINALLY {
        finally_called = 2; // This block should always be executed
    }
    BUT_END_TRY;

    BUT_ASSERT(catch_called == 1);
    BUT_ASSERT(finally_called == 2);
}

BUT_SUITE_BEGIN(Exceptions)
BUT_SUITE_ADD(test_throw)
BUT_SUITE_ADD(test_throw_file_line)
BUT_SUITE_ADD(test_throw_details)
BUT_SUITE_ADD(test_throw_details_file_line)
BUT_SUITE_ADD(test_throw_va)
BUT_SUITE_ADD(test_throw_va_file_line)
BUT_SUITE_ADD(test_throw_rethrow)
BUT_SUITE_ADD(test_throw_catch_all)
BUT_SUITE_ADD(test_failure)
BUT_SUITE_ADD(setup_failure)
BUT_SUITE_ADD(cleanup_failure)
BUT_SUITE_ADD(deep_nesting)
BUT_SUITE_ADD(deep_call_stack)
BUT_SUITE_ADD(finally_block)
BUT_SUITE_END;

BUT_GET_TEST_SUITE("Exceptions", Exceptions);
