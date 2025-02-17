#ifndef CRY_EXCEPTIONS_TYPES_H_
#define CRY_EXCEPTIONS_TYPES_H_

/**
 * @file types.h
 * @author Douglas Cuthbertson
 * @brief Type definitions for an exception handling library.
 * @version 0.1
 * @date 2023-12-03
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <setjmp.h> // jmp_buf

#if defined(__cplusplus)
extern "C" {
#endif

// The four states within an EXM_TRY block used to track whether a try-block
// was enter, an exception was thrown and not handled, caught and handled, and
// whether a final block was entered.
typedef enum EXMState {
    EXM_ENTERED,  // try block entered; setjmp has returned zero
    EXM_THROWN,   // exception thrown
    EXM_HANDLED,  // exception handled/caught
    EXM_FINALIZED // exception finalized
} EXMState;

/**
 * @brief The type of an exception. It is a constant string representing the reason the
 * exception was thrown (e.g., "out of memory", "invalid argument", or "just cause").
 */
typedef char const *EXMReason;

typedef struct EXMEnvironment {
    jmp_buf buf; ///< the jump buffer for setjmp/longjmp. Note that the offset of jmp_buf
                 ///< must be 16-byte aligned on some systems.
    struct EXMEnvironment
               *next; ///< a pointer to a parent context from an enclosing EXM_TRY block.
    EXMReason   reason;  ///< a constant string describing the reason for the exception.
    char const *details; ///< extra details about the exception
    char const *file;    ///< the name of the file from where the exception was thrown.
    int         line;    ///< the line number from where the exception was thrown.
    EXMState volatile state; ///< a try block is entered, thrown, handled, or finalized.
} EXMEnvironment;

/*
 * a handler should push top onto its stack if top is NOT null and
 * ignore the remaining arguments. Otherwise, it should assume an exception was
 * thrown and apply reason, path, and line to the top context. Of course, if
 * the stack is empty, then it should handle the exception and return.
 */

/**
 * @brief exm_closure the address of a function which takes an unspecified number of
 * arguments and has a void return value.
 *
 * Closure variables are actually pointers to an EXMClosure where the closure field is a
 * pointer to the handler function and the stack field is the address of to the top of a
 * stack of exception environments.
 *
 * The handler function will likely have several parameters. For example, the default
 * handler has 5 arguments: an exm_closure, an EXMReason, a possibly NULL "details"
 * string, the full path to the file in which the exception was thrown, and the line
 * number of the thrown exception.
 */
typedef void (*exm_closure)();

/**
 * @brief a pointer to an exception handling function.
 *
 * @param closure the address of a closure (handler function + context).
 * @param reason address of a string - it's the thrown exception.
 * @param details a possibly NULL address of a string with additional details about the
 * thrown exception.
 * @param file address of a string with the full path to the file in which the exception
 * was thrown.
 * @param line the line number of the thrown exception.
 */
typedef void (*exm_handler_fn)(exm_closure *closure, EXMReason reason,
                               char const *details, char const *file, int line);

/**
 * @brief The necessary data captured in EXM_TRY so exceptions can be thrown, and caught.
 *
 * Each EXM_TRY block creates a local EXMEnvironment, gets the address of the global
 * closure, and pushes the environment's address on to the closure's stack. If the
 * closure runs out of stack, then there are no more enclosing try/catch blocks with an
 * EXMEnvironment. That triggers exm_throw() to call the handler function.
 */
typedef struct EXMClosure {
    exm_handler_fn  handler; ///< exception handler
    EXMEnvironment *stack;   ///< top of a stack of exception environments
} EXMClosure;

typedef exm_closure *(*exm_register_closure_fn)(exm_closure *closure);

#if defined(__cplusplus)
}
#endif

#endif // CRY_EXCEPTIONS_TYPES_H_
