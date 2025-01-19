/**
 * @file assert.c
 * @author Douglas Cuthbertson
 * @brief Assertions based on an exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <exceptions/assert.h>
#include <exceptions/exceptions.h> // exm_throw
#include <size.h>                  // (KIBI(V)

#include <stdio.h>  // sprintf_s()
#include <assert.h> // assert()

// The full path to the file that throws an assertion failure is included in
// the assertion. This buffer is large enough to hold most paths. Expand it as
// needed.
GLOBAL_VARIABLE THREAD_LOCAL char buf[KIBI(4)];

// Use parentheses around the name "assert" to suppress expansion of the macro
// "assert" so we can actually define the function.
void(assert)(int e) {
    assert(e);
}

void exm_throw_assertion(char const *str, char const *file, int line) {
    sprintf_s(buf, sizeof buf, "assertion \"%s\"", str);
    exm_throw(buf, 0, file, line);
}
