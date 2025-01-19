#ifndef CRY_EXCEPTIONS_EXCEPTIONS_H_
#define CRY_EXCEPTIONS_EXCEPTIONS_H_

/**
 * @file exceptions.h
 * @author Douglas Cuthbertson
 * @brief
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include "types.h" // EXMReason, EXMClosure, exm_closure

#include <project.h> // CONTAINER_OF

#include <stdio.h> // snprintf

// This implementation of exception handling is inspired by and patterned after
// Hanson D. R. (1996). C Interfaces And Implementations: Techniques For
// Creating Reusable Software (pp.45-63). Addison-Wesley.

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief exm_not_implemented is useful during development.
 */
extern EXMReason exm_not_implemented;

/**
 * @brief exm_expected_failure is useful during testing. It's meant to be thrown and
 * caught without causing a *real* failure in the test driver.
 */
extern EXMReason exm_expected_failure;

/**
 * @brief a function has been passed an argument with an invalid value.
 */
extern EXMReason exm_invalid_value;

/**
 * @brief a component/library is in bad state. This probably means there's a
 * bug in it.
 */
extern EXMReason exm_internal_error;

//
// Exported macros for exception handling
//

/**
 * @brief
 *
 */

/**
 * @brief EXM_THROW throws an exception with the given reason and the file name and line
 * number on which it was called.
 */
#define EXM_THROW(reason) exm_throw((reason), 0, __FILE__, __LINE__)

#define EXM_THROW_FILE_LINE(reason, file, line) exm_throw((reason), 0, file, line)

/**
 * @brief EXM_THROW_DETAILS throws reason with details.
 *
 * There is a limit of 256 bytes in the buffer used for formatting the reason.
 *
 * @param reason is the EXMReason string that briefly describes the exception.
 * @param details a string that adds details to the reason why the exception was thrown.
 */
#define EXM_THROW_DETAILS(reason, details)             \
    do {                                               \
        static char _buf[256];                         \
        snprintf(_buf, sizeof _buf, details);          \
        exm_throw((reason), _buf, __FILE__, __LINE__); \
    } while (0)

#define EXM_THROW_DETAILS_FILE_LINE(reason, details, file, line) \
    do {                                                         \
        static char _buf[256];                                   \
        snprintf(_buf, sizeof _buf, details);                    \
        exm_throw((reason), _buf, file, line);                   \
    } while (0)

/**
 * @brief EXM_THROW_VA throws an exception where the reason is a format string and the
 * arguments match the format specifiers.
 *
 * There is a limit of 256 bytes in the buffer used for formatting the reason.
 *
 * @param reason is the EXMReason string that briefly describes the exception.
 * @param fmt is a C language printf-style format string for the var args.
 *
 */
#define EXM_THROW_VA(reason, fmt, ...)                       \
    do {                                                     \
        static char details[256];                            \
        snprintf(details, sizeof details, fmt, __VA_ARGS__); \
        exm_throw((reason), details, __FILE__, __LINE__);    \
    } while (0)

/**
 * @brief EXM_THROW_VA throws an exception where the reason is a format string and the
 * arguments match the format specifiers.
 *
 * There is a limit of 256 bytes in the buffer used for formatting the reason.
 *
 * @param reason is the EXMReason string that briefly describes the exception.
 * @param file is the path to the source file where the exception was thrown.
 * @param line is the line number in the source file where the exception was thrown.
 * @param fmt is a C language printf-style format string for the var args.
 *
 */
#define EXM_THROW_VA_FILE_LINE(reason, file, line, fmt, ...) \
    do {                                                     \
        static char details[256];                            \
        snprintf(details, sizeof details, fmt, __VA_ARGS__); \
        exm_throw((reason), details, file, line);            \
    } while (0)

/**
 * @brief EXM_RETHROW throws the last exception caught in an EXM_CATCH or
 * EXM_CATCH_ALL block.
 */
#define EXM_RETHROW \
    exm_throw(exm_env_.reason, exm_env_.details, exm_env_.file, exm_env_.line)

#define EXM_REASON (exm_env_.reason)
#define EXM_PATH   (exm_env_.file)
#define EXM_LINE   (exm_env_.line)

/**
 * @brief Start a new try/catch section.
 *
 * EXM_TRY creates a local EXMClosure on the program stack and pushes that to
 * the top of the exception stack (exm_stack). It then calls setjmp to
 * initialize the local frame's jump buffer. Finally, it compares setjmp's
 * return value in exm_ctx_->state to EXM_ENTERED (which is zero). If that's
 * so, then execution continues with the code in the EXM_TRY block.
 *
 * If setjmp returns a non-zero value (which should be EXM_THROWN), then an
 * exception has been thrown via a call to longjmp (see the implementation of
 * exm_throw in exceptions.c). In this case, if any EXM_CATCH blocks are
 * executed and match the thrown exception, or if an EXM_CATCH_ALL block
 * exists, then exm_ctx_->state is set to EXM_HANDLED, the handle-code is run.
 */

#define EXM_TRY                                                          \
    do {                                                                 \
        EXMEnvironment exm_env_ = {0};                                   \
        exm_closure   *cl       = exm_get_closure();                     \
        EXMClosure    *exm_ctx_ = CONTAINER_OF(cl, EXMClosure, handler); \
        exm_env_.next           = exm_ctx_->stack;                       \
        exm_ctx_->stack         = &exm_env_;                             \
        exm_env_.state          = setjmp(exm_env_.buf);                  \
        if (exm_env_.state == EXM_ENTERED) {
/**
 * @brief EXM_CATCH will catch an exception that matches its argument.
 *
 * If an exception was thrown, the value of exm_env_.state will be EXM_THROWN, so a catch
 * block will test to see if the reason in the local frame matches the given reason. If
 * they match, the exception is handled and the code for the catch block is executed.
 *
 * If the reason doesn't match, then execution continues on to the next catch, catch all,
 * finally block(s) until either the exception is handled or the end-try block is
 * executed (where upon the exception will be rethrown to the next exception frame up the
 * exception stack).
 */
#define EXM_CATCH(exception)                     \
    if (exm_env_.state == EXM_ENTERED) {         \
        exm_ctx_->stack = exm_ctx_->stack->next; \
    }                                            \
    }                                            \
    else if (exm_env_.reason == (exception)) {   \
        exm_env_.state = EXM_HANDLED;

/**
 * @brief EXM_CATCH_ALL catches any exception that hasn't already been caught.
 *
 * Catch any and all exceptions, mark them as handled and execute the code in
 * the catch-all block.
 */
#define EXM_CATCH_ALL                            \
    if (exm_env_.state == EXM_ENTERED) {         \
        exm_ctx_->stack = exm_ctx_->stack->next; \
    }                                            \
    }                                            \
    else {                                       \
        exm_env_.state = EXM_HANDLED;

/**
 * @brief EXM_FINALLY will run any code in this block regardless of whether an
 * exception has been thrown and regardless of whether a thrown exception has
 * been caught.
 */
#define EXM_FINALLY                              \
    if (exm_env_.state == EXM_ENTERED) {         \
        exm_ctx_->stack = exm_ctx_->stack->next; \
    }                                            \
    }                                            \
    {                                            \
        if (exm_env_.state == EXM_ENTERED) {     \
            exm_env_.state = EXM_FINALIZED;      \
        }

/**
 * @brief EXM_END_TRY ends a try/catch section.
 *
 * If an exception was thrown and it wasn't caught, it will rethrow it, passing
 * it to the next frame in the stack.
 */
#define EXM_END_TRY                              \
    if (exm_env_.state == EXM_ENTERED) {         \
        exm_ctx_->stack = exm_ctx_->stack->next; \
    }                                            \
    }                                            \
    if (exm_env_.state == EXM_THROWN) {          \
        EXM_RETHROW;                             \
    }                                            \
    }                                            \
    while (0)

/**
 * @brief Use the EXM_RETURN macro inside EXM_TRY blocks instead of "return".
 *
 * The switch statement ensures both "EXM_RETURN;" and "EXM_RETURN x;" expand
 * into a syntactically correct C statement.
 */
#define EXM_RETURN                                \
    switch ((*exm_stack) = (*exm_stack)->next, 0) \
    default:                                      \
        return

/**
 * @brief Use EXM_END_TRY_RETURN in try-catch sections when EXM_RETURN is used.
 * It prevents compilers from issuing the "unreachable code" warning that
 * EXM_END_TRY causes.
 */
#define EXM_END_TRY_RETURN \
    }                      \
    }                      \
    while (0)

/**
 * @brief handle a thrown exception.
 *
 * @param reason a string describing the exception that was thrown.
 * @param path the path to the file in which the exception was thrown.
 * @param line the line number on which the exception was thrown.
 */
extern void exm_throw(EXMReason reason, char const *details, char const *path, int line);

extern DATA_SPEC_EXPORT exm_closure *exm_get_closure();

extern DATA_SPEC_EXPORT exm_closure *exm_register_closure(exm_closure *closure);

#if defined(__cplusplus)
}
#endif

#endif // CRY_EXCEPTIONS_EXCEPTIONS_H_
