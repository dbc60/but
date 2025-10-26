# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Basic Unit Test (BUT) is a C unit test framework with the following architecture:

- **Test Driver** (`but.exe`): Loads test suite DLLs and executes their tests
- **Test Suites**: Compiled as DLLs that export a `BUTTestSuite *get_test_suite()` function
- **Exception Library**: Setjmp/longjmp-based exception handling for C (inspired by Hanson's "C Interfaces and Implementations")

**Key Design Principles:**
- Test suites are dynamically loaded shared libraries (DLLs/.so files)
- Exception handling uses thread-local storage and maintains an exception stack
- Exception addresses differ between executables and DLLs, so string comparison is used (`strcmp`) instead of pointer comparison
- Tests fail by throwing exceptions; setup/cleanup failures are tracked separately

## Build System

### Windows with Visual Studio

The primary build system uses batch scripts in `build/cmd/`:

**Main Scripts:**
- `all.cmd` - Builds exception library, BUT driver, and all tests
- `exception.cmd` - Builds the exception library only
- `but.cmd` - Builds the BUT driver only

**Common Options:**
```batch
# Build and run all tests (default: debug, x64)
build\cmd\all.cmd test

# Clean and rebuild in release mode
build\cmd\all.cmd test release clean

# Build 32-bit release
build\cmd\all.cmd release x86

# Specify VS version
build\cmd\all.cmd vs2022 test
```

**Available Commands:**
- `build` - Build the project (default)
- `debug` - Debug build without optimizations (default)
- `release` - Release build with optimizations
- `test` - Build and run unit tests
- `clean` - Delete build artifacts for current config
- `cleanall` - Delete all build artifacts
- `cleanplat` - Delete artifacts for current platform
- `x64` - 64-bit build (default)
- `x86` / `win32` - 32-bit build
- `vs2017` / `vs2019` / `vs2022` - Select VS version
- `verbose` - Show detailed build steps
- `trace` - Show option values
- `timed` - Run with ctime metrics (requires ctime.exe)

**Build Artifacts:**
All outputs go to `target\{compiler}\{platform}\{config}\`:
- `bin\` - Executables and DLLs
- `obj\` - Object files
- `inc\` - Generated headers

### Running Tests

```batch
# From repository root - builds and runs all tests
build\cmd\all.cmd test

# Run driver directly (from target\vs2022\x64\debug\bin\)
but.exe exception_butts.dll but_butts.dll
```

## Code Architecture

### Exception Handling System

**Core Concepts:**
- Uses `setjmp`/`longjmp` for non-local control flow
- Maintains a thread-local exception stack (`BUTExceptionContext.stack`)
- Each `BUT_TRY` block pushes a new `BUTExceptionEnvironment` onto the stack
- Throwing an exception longjmps to the nearest handler

**Exception Macros:**
```c
BUT_TRY {
    // Code that may throw
}
BUT_CATCH(specific_exception) {
    // Handle specific exception
    // Access: BUT_REASON, BUT_DETAILS, BUT_FILE, BUT_LINE
}
BUT_CATCH_ALL {
    // Handle any unhandled exception
    BUT_RETHROW;  // Re-throw if needed
}
BUT_FINALLY {
    // Always executes (like finally in other languages)
}
BUT_END_TRY;
```

**Special Cases:**
- **IMPORTANT:** Use `BUT_RETURN` instead of `return` inside `BUT_TRY` blocks to properly unwind the exception stack
- Use `BUT_END_TRY_RETURN` instead of `BUT_END_TRY` when `BUT_RETURN` is used (prevents "unreachable code" warnings)
- Do NOT add early-exit return statements in code wrapped in `BUT_TRY`/`BUT_END` blocks - they create unreachable code sections

**Throwing Exceptions:**
```c
BUT_THROW(exception_reason);
BUT_THROW_DETAILS(reason, "format string %d", value);
BUT_RETHROW;  // Inside catch blocks only
```

**Predefined Exceptions:**
- `but_expected_failure` - Test intentionally failed (not reported as error)
- `but_unexpected_failure` - General test failure
- `but_test_exception` - For testing the exception system itself
- `but_not_implemented` - Feature not yet implemented
- `but_invalid_value` - Invalid argument passed to function
- `but_internal_error` - Bug in the library
- `but_invalid_address` - Invalid memory address

### Test Suite Structure

**Defining Tests:**
```c
// Simple test with no setup/cleanup
BUT_TEST("Test Name", test_function_name) {
    BUT_ASSERT_TRUE(condition);
    BUT_ASSERT_EQ(expected, actual);
}

// Test with setup and cleanup
BUT_TEST_SETUP_CLEANUP("Test Name", test_func, setup_func, cleanup_func) {
    // Test code
}

// Test with custom type (embeds BUTTestCase)
typedef struct {
    BUT_EMBED_CASE;  // Must be first member
    // Custom test data
} MyTestData;

BUT_TYPE_TEST_SETUP_CLEANUP("Test", MyTestData, test_func, setup_func, cleanup_func) {
    // 't' parameter is MyTestData*
    BUT_ASSERT_NOT_NULL(t->custom_field);
}
```

**Registering Test Suite:**
```c
// Define array of test cases
BUT_SUITE_BEGIN(my_suite)
    BUT_SUITE_ADD(simple_test)              // For BUT_TEST
    BUT_SUITE_ADD_EMBEDDED(typed_test)      // For BUT_TYPE_TEST_*
BUT_SUITE_END;

// Create test suite and export function
BUT_GET_TEST_SUITE("Suite Name", my_suite);
```

**Helper Macros:**
- `BUT_UNUSED(param)` - Mark parameter as intentionally unused
- `BUT_UNUSED_TYPE_ARG` - Mark test case parameter as unused in typed tests
- `BUT_CONTAINER(ptr, type, member)` - Get containing struct (like Linux's `container_of`)
- `BUT_ARRAY_COUNT(array)` - Number of elements in fixed array

### Assertion Macros

**Boolean:**
- `BUT_ASSERT_TRUE(expr)`, `BUT_ASSERT_FALSE(expr)`

**Integers (type-specific):**
- `BUT_ASSERT_EQ_INT`, `BUT_ASSERT_NEQ_INT`, `BUT_ASSERT_LT_INT`, etc.
- `BUT_ASSERT_EQ_UINT`, `BUT_ASSERT_EQ_LONG`, `BUT_ASSERT_EQ_SIZE_T`
- `BUT_ASSERT_EQ_UINT32`, `BUT_ASSERT_EQ_INT64`, `BUT_ASSERT_EQ_UINTPTR`
- `BUT_ASSERT_ZERO(value)`, `BUT_ASSERT_NON_ZERO(value)`

**Generic (requires C11 `_Generic`):**
- `BUT_ASSERT_EQ`, `BUT_ASSERT_NEQ`, `BUT_ASSERT_LT`, `BUT_ASSERT_LE`, `BUT_ASSERT_GT`, `BUT_ASSERT_GE`
- Automatically dispatches to typed version based on argument type

**Pointers:**
- `BUT_ASSERT_NULL(ptr)`, `BUT_ASSERT_NOT_NULL(ptr)`
- `BUT_ASSERT_EQ_PTR(expected, actual)`, `BUT_ASSERT_NEQ_PTR`

**Strings:**
- `BUT_ASSERT_STREQ(expected, actual)`, `BUT_ASSERT_STRNEQ`

**Memory & Floating-Point:**
- `BUT_ASSERT_MEM_EQ(expected, actual, size)`, `BUT_ASSERT_MEM_NEQ`
- `BUT_ASSERT_FLOAT_EQ(expected, actual, epsilon)`
- `BUT_ASSERT_DOUBLE_EQ(expected, actual, epsilon)`

**Custom Assertions:**
Define custom type-specific assertions by setting `BUT_ASSERT_EQ_CUSTOM` before including headers:
```c
#define BUT_ASSERT_EQ_CUSTOM MyCustomType: MY_ASSERT_EQ, default: BUT_ASSERT_EQ_INT
```

### Test Driver API

**Context Management:**
```c
BUTContext ctx;
but_initialize(&ctx, handler_fn);  // Initialize with exception handler
but_begin(&ctx, test_suite);       // Start test suite
but_end(&ctx);                     // Cleanup (frees results)
```

**Iteration:**
```c
while (but_has_more(&ctx)) {
    but_driver(&ctx);              // Run current test
    but_next(&ctx);                // Move to next test
}
```

**Results:**
```c
but_get_run_count(&ctx)            // Tests executed
but_get_pass_count(&ctx)           // Tests passed
but_get_test_failure_count(&ctx)   // Tests failed
but_get_setup_failure_count(&ctx)  // Setup failures
but_get_cleanup_failure_count(&ctx) // Cleanup failures
but_get_result(&ctx, index)        // Result for specific test
```

### Platform-Specific Macros

**DLL Export/Import:**
- `DLL_SPEC_EXPORT` - Export symbol from DLL
- `DLL_SPEC_IMPORT` - Import symbol from DLL
- `DLL_SPEC` - Controlled by `DLL_BUILD` define

**Calling Conventions:**
- `STDCALL` - `__stdcall` on Windows
- `CDECL` - `__cdecl` on Windows

**Thread-Local Storage:**
- `THREAD_LOCAL` - Expands to appropriate keyword for platform/standard

## Important Constraints

1. **Exception String Comparison**: Always use `strcmp` to compare exception reasons, never pointer comparison. This is because exception addresses differ between the driver executable and test DLLs.

2. **Return Statements**: Never use plain `return` inside `BUT_TRY` blocks. Use `BUT_RETURN` and `BUT_END_TRY_RETURN` instead to properly unwind exception stack.

3. **Static Linking**: The exception library is statically linked to both executables and DLLs, which is why exception addresses are not comparable across module boundaries.

4. **Test Case First Member**: When embedding `BUTTestCase` in custom structs, it must be the first member (use `BUT_EMBED_CASE` macro).

5. **Expected vs Unexpected Failures**: Use `but_expected_failure` for tests that are meant to demonstrate expected error handling. Use `but_unexpected_failure` (or assertion macros) for actual test failures.

## Development Workflow

1. **Write Tests**: Create `.c` file with test cases using `BUT_TEST` macros
2. **Register Suite**: Use `BUT_SUITE_BEGIN/END` and `BUT_GET_TEST_SUITE`
3. **Build as DLL**: Configure build to export `get_test_suite()` function
4. **Run Driver**: Execute `but.exe <test_suite.dll>` to run tests
5. **Debug Failures**: Exception details include file/line information

## File Organization

- `include/` - Public headers (`but.h`, `but_macros.h`, `but_assert.h`, `exception.h`)
- `src/` - Implementation files and private headers
- `cmd/but/` - Test driver main entry point
- `build/cmd/` - Windows build scripts
- `target/` - Build output directory (generated)
