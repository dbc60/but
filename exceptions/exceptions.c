/**
 * @file exceptions.c
 * @author Douglas Cuthbertson
 * @brief An exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <exceptions/exceptions.h> // exm_throw
#include <exceptions/types.h>      // EXMReason, EXMClosure, EXM_THROWN
#include <exceptions/assert.h>     // assert
#include <project.h>               // INTERNAL_FUNCTION, GLOBAL_VARIABLE, etc.

#include <stddef.h> // offsetof, NULL
#include <stdio.h>  // fprintf
#include <stdlib.h> // abort

EXMReason exm_not_implemented  = "not implemented";
EXMReason exm_expected_failure = "expected failure";
EXMReason exm_invalid_value    = "invalid value";
EXMReason exm_internal_error   = "internal error";

/**
 * @brief The default handler (through a pointer that can be updated to
 * point to an application-specific handler) when an exception is not caught.
 *
 * @param closure
 * @param reason a brief reason for throwing the exception.
 * @param details a (possibly NULL) string providing more details about the exception.
 * @param file the file name in which the exception was thrown.
 * @param line the line number of the file on which the exception was thrown.
 */
INTERNAL_FUNCTION void default_handler(exm_closure *closure, EXMReason reason,
                                       char const *details, char const *file, int line) {
    EXMClosure *ctx = CONTAINER_OF(closure, EXMClosure, handler);

    if (details == NULL) {
        fprintf(stderr, "unexpected exception thrown: ctx(0x%p): reason(%s), @%s:%d\n",
                ctx, reason, file, line);
    } else {
        fprintf(
            stderr,
            "unexpected exception thrown: ctx(0x%p): reason(%s), details(%s), @%s:%d\n",
            ctx, reason, details, file, line);
    }
}

GLOBAL_VARIABLE EXMClosure default_closure_ = {default_handler};

/*
 * N.B.: If a shared library (DLL, .so) spawns threads, each thread will have its own
 * instance of closure_. If you want those threads to use a closure other than the
 * default, then they will each have to register it for themselves either directly, or by
 * calling exm_register_closure(exm_closure *closure) defined below.
 */
GLOBAL_VARIABLE THREAD_LOCAL exm_closure *closure_ = (exm_closure *)&default_closure_;

/**
 * @brief retrieve the address of the per-thread exception closure.
 *
 * @return the address of the per-thread exception closure.
 */
DATA_SPEC_EXPORT
exm_closure *exm_get_closure() {
    return closure_;
}

void exm_throw(EXMReason reason, char const *details, char const *file, int line) {
    assert(reason);
    exm_closure *cp      = exm_get_closure();
    EXMClosure  *closure = CONTAINER_OF(cp, EXMClosure, handler);
    if (closure->stack == NULL) {
        // handle an unhandled exception
        (*closure->handler)(cp, reason, details, file, line);
    } else {
        // pop the stack and leap to the next try-catch block
        EXMEnvironment *env = closure->stack;
        closure->stack      = closure->stack->next;
        env->reason         = reason;
        env->details        = details;
        env->file           = file;
        env->line           = line;
        longjmp(env->buf, EXM_THROWN);
    }
}

/**
 * @brief Enable an application to register its own handler for uncaught
 * exceptions.
 *
 * It accepts a new handler context and returns the previous one.
 */
DATA_SPEC_EXPORT
exm_closure *exm_register_closure(exm_closure *closure) {
    assert(closure != NULL);
    exm_closure *previous = closure_;
    closure_              = closure;

    return previous;
}
