/**
 * @file exceptions.c
 * @author Douglas Cuthbertson
 * @brief An exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <but_macros.h>       // THREAD_LOCAL,BUT_INTERNAL, BUT_GLOBAL, etc.
#include <exception.h>        // but_throw
#include <exception_types.h>  // BUTExceptionReason, BUTExceptionContext, BUT_THROWN
#include <exception_assert.h> // assert
#include "log.h"

#include <stddef.h>  // offsetof, NULL
#include <stdio.h>   // fprintf
#include <stdlib.h>  // abort
#include <string.h>  // strncpy_s
#include <threads.h> // mtx_t, mtx_lock, mtx_unlock

BUTExceptionReason but_expected_failure
    = "expected failure"; ///< test drivers catch this and do not report it as a failure
BUTExceptionReason but_unexpected_failure
    = "unexpected failure"; ///< used when an address is not valid
BUTExceptionReason but_test_exception
    = "test exception"; ///< used for tests that need to throw and catch an exception
BUTExceptionReason but_not_implemented = "not implemented"; ///< used during development
BUTExceptionReason but_invalid_value
    = "invalid value"; ///< used when an argument is invalid
BUTExceptionReason but_internal_error
    = "internal error"; ///< used when a component/library is in a bad state
BUTExceptionReason but_invalid_address
    = "invalid address"; ///< used when an address is not valid

/**
 * @brief The default handler (through a pointer that can be updated to
 * point to an application-specific handler) when an exception is not caught.
 *
 *  void but_default_handler(but_handler_fn **handler, BUTExceptionReason reason, char
 * const *details, char const *file, int line)
 *
 * @param handler address of the handler in an BUTExceptionContext
 * @param reason a brief reason for throwing the exception.
 * @param details a (possibly NULL) string providing more details about the exception.
 * @param file the file name in which the exception was thrown.
 * @param line the line number of the file on which the exception was thrown.
 */
BUT_HANDLER_FN(but_default_handler) {
    if (details == NULL) {
        fprintf(stderr, "unexpected exception thrown: ctx(0x%p): reason(%s), @%s:%d\n",
                ctx, reason, file, line);
    } else {
        fprintf(
            stderr,
            "unexpected exception thrown: ctx(0x%p): reason(%s), details(%s), @%s:%d\n",
            ctx, reason, details, file, line);
    }
    fflush(stderr);
    abort(); // terminate the program
}

static THREAD_LOCAL BUTExceptionContext g_default_context_
    = {.handler = but_default_handler, .stack = NULL};

/*
 * N.B.: If a shared library (DLL, .so) spawns threads, each thread will have its own
 * instance of g_handler_. If you want those threads to use a handler other than the
 * default, then they will each have to register it for themselves either directly, or by
 * calling but_set_exception_context(but_handler_fn **handler) defined below.
 *
 * The global context works only in single-threaded contexts. Maybe I should get rid of
 * the global context and assert that g_context_ is NOT NULL before each use. It would
 * require a call to but_set_exception_context() for initialization.
 */
static THREAD_LOCAL BUTExceptionContext *g_context_;
static once_flag                         g_context_init_flag_ = ONCE_FLAG_INIT;

BUT_INIT_FN(but_init) {
    assert(ctx);
    ctx->handler = handler;
    ctx->stack   = NULL;
}

void PrintCallingModule() {
    HMODULE hModule;
    char    modulePath[MAX_PATH];

    logger_init();
    // Get module handle for current function's address
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                      (LPCTSTR)PrintCallingModule, &hModule);

    GetModuleFileNameA(hModule, modulePath, MAX_PATH);
    char const *file = logger_get_filename(modulePath);
    // Check if it ends with .exe or .dll
    char *extension = strrchr(file, '.');
    if (extension && _stricmp(extension, ".exe") == 0) {
        LOG_TRACE("exception", "Called from executable: %s", file);
    } else if (extension && _stricmp(extension, ".dll") == 0) {
        LOG_TRACE("exception", "Called from DLL: %s", file);
    }
}

BUT_THROW_FN(but_throw) {
    assert(reason);
    PrintCallingModule();
    BUTExceptionContext *ctx = but_get_exception_context(__FILE__, __LINE__);
    if (ctx->stack == NULL) {
        // handle an unhandled exception
        (*ctx->handler)(ctx, reason, details, file, line);
    } else {
        // pop the stack and leap to the next try-catch block
        BUTExceptionEnvironment *env = ctx->stack;
        ctx->stack                   = ctx->stack->next;
        env->reason                  = reason;
        env->details                 = details;
        env->file                    = file;
        env->line                    = line;
        longjmp(env->jmp, BUT_THROWN);
    }
}

void initialize_g_context_once(void) {
    if (g_context_ == 0) {
        g_context_ = &g_default_context_;
    }
}

/**
 * @brief retrieve the address of the per-thread exception handler.
 *
 * @return the address of the per-thread exception handler.
 */
DLL_SPEC_EXPORT BUT_GET_EXCEPTION_CONTEXT(but_get_exception_context) {
    call_once(&g_context_init_flag_, initialize_g_context_once);
    LOG_TRACE_FILE_LINE("Exception", file, line, "context: 0x%p, %s[%d]", g_context_,
                        file, line);
    return g_context_;
}

/**
 * @brief Enable an application to register its own handler for uncaught exceptions. It
 * accepts an exception handler (handler context) and returns the previous one.
 *
 * @return the previous handler context.
 */
DLL_SPEC_EXPORT
BUT_SET_EXCEPTION_CONTEXT(but_set_exception_context) {
    LOG_DEBUG_FILE_LINE("Exception", file, line, "replace 0x%p wit 0x%p", g_context_,
                        ctx);
    assert(ctx != NULL);
    BUTExceptionContext *previous;

    call_once(&g_context_init_flag_, initialize_g_context_once);
    previous   = g_context_;
    g_context_ = ctx;

    return previous;
}
