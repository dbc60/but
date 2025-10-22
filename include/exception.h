#ifndef BUT_EXCEPTION_H_
#define BUT_EXCEPTION_H_

/**
 * @file exception.h
 * @author Douglas Cuthbertson
 * @brief
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "exception_types.h" // BUTExceptionReason, BUTExceptionContext, but_handler_fn

#include <abbreviated_types.h> // u64
#include <but_macros.h>        // THREAD_LOCAL

#include <stdio.h> // snprintf

// This implementation of exception handling is inspired by and patterned after
// Hanson D. R. (1996). C Interfaces And Implementations: Techniques For
// Creating Reusable Software (pp.45-63). Addison-Wesley.

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief but_expected_failure is meant to be thrown from test cases, but not caught
 * within the test. The test driver should catch it and NOT report it as a failed test
 * case.
 */
extern BUTExceptionReason but_expected_failure;

/**
 * @brief this is primarily used in various BUT_ASSERT macros to indicate an unexpected
 * failure.
 */
extern BUTExceptionReason but_unexpected_failure;

/**
 * @brief but_test_exception is useful during testing. It's meant to be thrown and caught
 * within test cases. If it is not caught, then the test driver will report it as an
 * uncaught exception. It's mostly used within the test suite for the exception library.
 */
extern BUTExceptionReason but_test_exception;

/**
 * @brief but_not_implemented is useful during development to track unimplemented
 * features.
 */
extern BUTExceptionReason but_not_implemented;

/**
 * @brief but_invalid_value can be thrown by a function that has been passed an argument
 * with an invalid value.
 */
extern BUTExceptionReason but_invalid_value;

/**
 * @brief but_internal_error can be thrown by a function that is in a bad state. This
 * probably means there's a bug in its component/library.
 */
extern BUTExceptionReason but_internal_error;

/**
 * @brief but_invalid_address can be thrown by a function that has been passed an invalid
 * address, such as one that has already been freed or one that is not within the bounds
 * of a valid memory region.
 */
extern BUTExceptionReason but_invalid_address;

//
// Exported macros for exception handling
//

/**
 * @brief BUT_THROW throws an exception with the given reason and the file name and line
 * number on which it was called.
 */
#define BUT_THROW(reason) but_throw((reason), 0, __FILE__, __LINE__)

#define BUT_THROW_FILE_LINE(reason, file, line) but_throw((reason), 0, file, line)
/**
 * @brief BUT_THROW_DETAILS throws reason with details.
 *
 * There is a limit of BUT_MAX_DETAILS_LENGTH bytes in the buffer used for formatting the
 * reason.
 *
 * @param reason is the BUTExceptionReason string that briefly describes the exception.
 * @param details a string that adds details to the reason why the exception was thrown.
 * If there are arguments passed after details, then the details string is a format
 * string.
 */
#define BUT_THROW_DETAILS(reason, details, ...)                      \
    do {                                                             \
        static THREAD_LOCAL char _details[BUT_MAX_DETAILS_LENGTH];   \
        snprintf(_details, sizeof _details, details, ##__VA_ARGS__); \
        but_throw((reason), _details, __FILE__, __LINE__);           \
    } while (0)

/**
 * @brief throw an exception where the details is a format string with optional format
 * specifiers.
 *
 * There is a limit of BUT_MAX_DETAILS_LENGTH bytes in the buffer used for formatting the
 * reason.
 *
 * @param reason is the BUTExceptionReason string that briefly describes the exception.
 * @param details  a string that adds details to the reason why the exception was thrown.
 * If there are arguments passed after details, then the details string must have format
 * specifiers to match.
 * @param file is the path to the source file where the exception was thrown.
 * @param line is the line number in the source file where the exception was thrown.
 */
#define BUT_THROW_DETAILS_FILE_LINE(reason, details, file, line, ...) \
    do {                                                              \
        static THREAD_LOCAL char _details[BUT_MAX_DETAILS_LENGTH];    \
        snprintf(_details, sizeof _details, details, ##__VA_ARGS__);  \
        but_throw((reason), _details, file, line);                    \
    } while (0)

/**
 * @brief BUT_RETHROW throws the last exception caught in an BUT_CATCH or
 * BUT_CATCH_ALL block.
 */
#define BUT_RETHROW \
    but_throw(but_env_.reason, but_env_.details, but_env_.file, but_env_.line)

#define BUT_REASON  (but_env_.reason)
#define BUT_DETAILS (but_env_.details)
#define BUT_FILE    (but_env_.file)
#define BUT_LINE    (but_env_.line)

/**
 * @brief Start a new try/catch section.
 *
 * BUT_TRY creates a local BUTExceptionContext on the program stack and pushes that to
 * the top of the exception stack (but_stack). It then calls setjmp to
 * initialize the local frame's jump buffer. Finally, it compares setjmp's
 * return value in but_ctx_->state to BUT_ENTERED (which is zero). If that's
 * so, then execution continues with the code in the BUT_TRY block.
 *
 * If setjmp returns a non-zero value (which should be BUT_THROWN), then an
 * exception has been thrown via a call to longjmp (see the implementation of
 * but_throw in exceptions.c). In this case, if any BUT_CATCH blocks are
 * executed and match the thrown exception, or if an BUT_CATCH_ALL block
 * exists, then but_ctx_->state is set to BUT_HANDLED, the handle-code is run.
 */
#define BUT_TRY                                                                           \
    do {                                                                                  \
        BUTExceptionEnvironment but_env_ = {0};                                           \
        but_env_.reason                  = but_not_implemented;                           \
        but_env_.try_file                = __FILE__;                                      \
        but_env_.try_line                = __LINE__;                                      \
        BUTExceptionContext *but_ctx_    = but_get_exception_context(__FILE__, __LINE__); \
        but_env_.next                    = but_ctx_->stack;                               \
        but_ctx_->stack                  = &but_env_;                                     \
        but_env_.state                   = setjmp(but_env_.jmp);                          \
        but_env_.h                       = GetModuleHandle(NULL);                         \
        if (but_env_.state == BUT_ENTERED) {
/**
 * @brief BUT_CATCH will catch an exception that matches its argument.
 *
 * If an exception was thrown, the value of but_env_.state will be BUT_THROWN, so a catch
 * block will test to see if the reason in the local frame matches the given reason. If
 * they match, the exception is handled and the code for the catch block is executed.
 *
 * If the reason doesn't match, then execution continues on to the next catch, catch all,
 * finally block(s) until either the exception is handled or the end-try block is
 * executed (where upon the exception will be rethrown to the next exception frame up the
 * exception stack).
 */
#define BUT_CATCH(exception)                     \
    if (but_env_.state == BUT_ENTERED) {         \
        but_ctx_->stack = but_ctx_->stack->next; \
    }                                            \
    }                                            \
    else if (but_env_.reason == (exception)) {   \
        but_env_.state = BUT_HANDLED;

/**
 * @brief BUT_CATCH_ALL catches any exception that hasn't already been caught.
 *
 * Catch any and all exceptions, mark them as handled and execute the code in
 * the catch-all block.
 */
#define BUT_CATCH_ALL                            \
    if (but_env_.state == BUT_ENTERED) {         \
        but_ctx_->stack = but_ctx_->stack->next; \
    }                                            \
    }                                            \
    else {                                       \
        but_env_.state = BUT_HANDLED;

/**
 * @brief BUT_FINALLY will run any code in this block regardless of whether an
 * exception has been thrown and regardless of whether a thrown exception has
 * been caught.
 */
#define BUT_FINALLY                              \
    if (but_env_.state == BUT_ENTERED) {         \
        but_ctx_->stack = but_ctx_->stack->next; \
    }                                            \
    }                                            \
    {                                            \
        if (but_env_.state == BUT_ENTERED) {     \
            but_env_.state = BUT_HANDLED;        \
        }

/**
 * @brief BUT_END_TRY ends a try/catch section.
 *
 * If an exception was thrown and it wasn't caught, it will rethrow it, passing
 * it to the next frame in the stack.
 */
#define BUT_END_TRY                              \
    if (but_env_.state == BUT_ENTERED) {         \
        but_ctx_->stack = but_ctx_->stack->next; \
    }                                            \
    }                                            \
    if (but_env_.state == BUT_THROWN) {          \
        BUT_RETHROW;                             \
    }                                            \
    }                                            \
    while (0)

/**
 * @brief Use the BUT_RETURN macro inside BUT_TRY blocks instead of "return".
 *
 * The switch statement ensures both "BUT_RETURN;" and "BUT_RETURN x;" expand
 * into a syntactically correct C statement.
 */
#define BUT_RETURN                                \
    switch ((*but_stack) = (*but_stack)->next, 0) \
    default:                                      \
        return

/**
 * @brief Use BUT_END_TRY_RETURN in try-catch sections when BUT_RETURN is used.
 * It prevents compilers from issuing the "unreachable code" warning that
 * BUT_END_TRY causes.
 */
#define BUT_END_TRY_RETURN \
    }                      \
    }                      \
    while (0)

#define BUT_INIT_FN(name) void name(BUTExceptionContext *ctx, but_handler_fn *handler)
typedef BUT_INIT_FN(but_init_fn);
extern BUT_INIT_FN(but_init);

/**
 * @brief handle a thrown exception.
 *
 * @param reason a string describing the exception that was thrown.
 * @param path the path to the file in which the exception was thrown.
 * @param line the line number on which the exception was thrown.
 */
#define BUT_THROW_FN(name) \
    void name(BUTExceptionReason reason, char const *details, char const *file, u32 line)
typedef BUT_THROW_FN(but_throw_fn);
extern BUT_THROW_FN(but_throw);
extern BUT_HANDLER_FN(but_default_handler);

extern DLL_SPEC_EXPORT BUT_GET_EXCEPTION_CONTEXT(but_get_exception_context);
extern DLL_SPEC_EXPORT BUT_SET_EXCEPTION_CONTEXT(but_set_exception_context);

#if defined(__cplusplus)
}
#endif

#endif // BUT_EXCEPTION_H_
