#ifndef BUT_H_
#define BUT_H_

/**
 * @file but.h
 * @author Douglas Cuthbertson
 * @brief definitions for Basic Unit Test (BUT) test cases and test suites.
 * @version 0.2
 * @date 2005-10-14
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#include <exceptions/exceptions.h> // exm_expected_failure
#include <platform.h>              // u32

#include <string.h> // strcmp

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief produce a non-zero value for any exception except exm_expected_failure.
 *
 * We use strcmp(), because the exception library is statically linked to both
 * executables (like a test driver) and shared libraries (like a test suite).
 * That means the address of an exception thrown by a shared library and caught
 * by a test driver won't be the same, but their string-values will be.
 */
#define BUT_UNEXPECTED_EXCEPTION(e) strcmp((e), exm_expected_failure)

// Forward declaration to suppress "warning C4115: 'BUTTestCase': named type
// definition in parentheses"
struct BUTTestCase;
struct BUTTestSuite;

// Setup is intended to initialize a test case by acquiring any necessary
// resources. If it fails to acquire those resources, it should throw an
// exception that includes the reason for the failure.
typedef void (*but_setup_fn)(struct BUTTestCase *);

// Run is the function that runs a test case. It should throw an
// exception on failure and the exception should state the reason for the
// failure.
typedef void (*but_test_fn)(struct BUTTestCase *);

// Cleanup is intended to release any resources acquired by but_setup.
typedef void (*but_cleanup_fn)(struct BUTTestCase *);

// A test case has a name, an optional setup function, a test function, and an
// optional cleanup function.
struct BUTTestCase {
    char          *name;
    but_setup_fn   setup;
    but_test_fn    test;
    but_cleanup_fn cleanup;
};
typedef struct BUTTestCase BUTTestCase;

// A test suite has a name and one or more test cases to run.
struct BUTTestSuite {
    char         *name;
    u32           count;
    BUTTestCase **test_cases;
};
typedef struct BUTTestSuite BUTTestSuite;

typedef BUTTestSuite *(*but_get_test_suite)();

#if defined(__cplusplus)
}
#endif

#endif // BUT_H_
