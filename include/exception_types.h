#ifndef BUT_EXCEPTION_TYPES_H_
#define BUT_EXCEPTION_TYPES_H_

/**
 * @file exception_types.h
 * @author Douglas Cuthbertson
 * @brief Type definitions for an exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <abbreviated_types.h> // u32
#include <setjmp.h>            // jmp_buf

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h> // GetModuleHandle()

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef BUT_MAX_DETAILS_LENGTH
///< the maximum length of details in an exception
#define BUT_MAX_DETAILS_LENGTH 512
#endif

// The four states within an BUT_TRY block used to track whether a try-block
// was enter, an exception was thrown and not handled, caught and handled, and
// whether a final block was entered.
typedef enum BUTExceptionState {
    BUT_ENTERED, // try block entered; setjmp has returned zero
    BUT_THROWN,  // exception thrown
    BUT_HANDLED, // exception handled/caught or finally block entered w/o throwing an
                 // exception
} BUTExceptionState;

/**
 * @brief The type of an exception. It is a constant string representing the reason the
 * exception was thrown (e.g., "out of memory", "invalid argument", or "just cause").
 */
typedef char const *BUTExceptionReason;

typedef struct BUTExceptionEnvironment {
    jmp_buf jmp; ///< the jump buffer for setjmp/longjmp. Note that the offset of jmp_buf
                 ///< must be 16-byte aligned on some systems.
    struct BUTExceptionEnvironment
        *next; ///< a pointer to a parent context from an enclosing BUT_TRY block.
    BUTExceptionReason
                reason;   ///< a constant string describing the reason for the exception.
    char const *details;  ///< extra details about the exception
    char const *try_file; ///< the file where the last setjmp was set.
    char const *file;     ///< the file where the exception was thrown.
    u32         try_line; ///< the line where the last setjmp was set.
    u32         line;     ///< the line where the exception was thrown.
    BUTExceptionState volatile state; ///< a try block is entered, thrown, handled, or
                                      ///< finalized.
    HMODULE h;                        ///< a handle to the current module
} BUTExceptionEnvironment;

/*
 * a handler should push top onto its stack if top is NOT null and
 * ignore the remaining arguments. Otherwise, it should assume an exception was
 * thrown and apply reason, path, and line to the top context. Of course, if
 * the stack is empty, then it should handle the exception and return.
 */

typedef struct BUTExceptionContext BUTExceptionContext;

/**
 * @brief a pointer to an exception handling function.
 *
 * @param ctx the address of an BUTExceptionContext (handler function + exception stack).
 * @param reason address of a string - it's the thrown exception.
 * @param details a possibly NULL address of a string with additional details about the
 * thrown exception.
 * @param file address of a string with the full path to the file in which the exception
 * was thrown.
 * @param line the line number of the thrown exception.
 */
#define BUT_HANDLER_FN(name)                                                            \
    void name(BUTExceptionContext *ctx, BUTExceptionReason reason, char const *details, \
              char const *file, int line)
typedef BUT_HANDLER_FN(but_handler_fn);
typedef but_handler_fn *but_handler;

/**
 * @brief The necessary data captured in BUT_TRY so exceptions can be thrown, and caught.
 *
 * Each BUT_TRY block creates a local BUTExceptionEnvironment, gets the address of the
 * global handler, and pushes the environment's address on to the handler's stack. If the
 * handler runs out of stack, then there are no more enclosing try/catch blocks with an
 * BUTExceptionEnvironment. That triggers but_throw() to call the handler function.
 */
struct BUTExceptionContext {
    but_handler_fn          *handler; ///< exception handler
    BUTExceptionEnvironment *stack;   ///< top of a stack of exception environments
};

#define BUT_GET_EXCEPTION_CONTEXT(name) \
    BUTExceptionContext *name(char const *file, int line)
typedef BUT_GET_EXCEPTION_CONTEXT(but_get_exception_context_fn);

#define BUT_SET_EXCEPTION_CONTEXT(name) \
    BUTExceptionContext *name(BUTExceptionContext *ctx, char const *file, int line)
typedef BUT_SET_EXCEPTION_CONTEXT(but_set_exception_context_fn);

#if defined(__cplusplus)
}
#endif

#endif // BUT_EXCEPTION_TYPES_H_
