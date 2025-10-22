/**
 * @file exception_assert.c
 * @author Douglas Cuthbertson
 * @brief Assertions based on an exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <exception_assert.h> // assert
#include <exception_types.h>  // BUTExceptionReason
#include <exception.h>        // but_throw
#include <but_macros.h>       // THREAD_LOCAL

#include <stdio.h> // snprintf()

BUTExceptionReason but_assertion_failure = "assertion failure";

// The full path to the file that throws an assertion failure is included in
// the assertion. This buffer is large enough to hold most paths. Expand it as
// needed.
void but_throw_assertion(char const *reason, char const *file, int line) {
    but_throw(but_assertion_failure, reason, file, line);
}
