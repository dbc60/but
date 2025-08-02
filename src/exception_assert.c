/**
 * @file assert.c
 * @author Douglas Cuthbertson
 * @brief Assertions based on an exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <exception_assert.h> // assert
#include <exception.h>        // but_throw
#include <but_macros.h>       // THREAD_LOCAL
#include <size.h>             // (KIBI(V)

#include <stdio.h> // snprintf()

// The full path to the file that throws an assertion failure is included in
// the assertion. This buffer is large enough to hold most paths. Expand it as
// needed.
BUT_GLOBAL THREAD_LOCAL char assert_buf[KIBI(4)];

void but_throw_assertion(char const *str, char const *file, int line) {
    snprintf(assert_buf, sizeof assert_buf, "assertion \"%s\"", str);
    but_throw(assert_buf, 0, file, line);
}
