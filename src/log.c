/**
 * @file log.c
 * @author Douglas Cuthbertson
 * @brief
 * @version 0.1
 * @date 2025-08-20
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 *
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include "but_macros.h"
#include "exception_assert.h"
#include "log.h"

#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

static _Atomic unsigned long       next_thread_id             = 1;
static _Thread_local unsigned long my_thread_id               = 0;
static once_flag                   g_logger_init_flag         = ONCE_FLAG_INIT;
static once_flag                   g_logger_context_init_flag = ONCE_FLAG_INIT;

static THREAD_LOCAL LoggerContext *g_logger_context_ = NULL;
static LoggerContext               g_default_logger_context_
    = {.logger = {.enabled      = true,
                  .min_level    = LOG_INFO,
                  .output       = NULL, // Will be set to stdout in init
                  .close_output = false},
       .name   = "default"};

// Level names for output
static char const *level_names[]
    = {"FATAL", "ERROR", "WARN", "INFO", "VERBOSE", "DEBUG", "TRACE"};

static void initialize_g_logger_context_once(void) {
    if (g_logger_context_ == NULL) {
        g_logger_context_ = &g_default_logger_context_;
        // Initialize the default logger's mutex
        if (g_default_logger_context_.logger.output == NULL) {
            g_default_logger_context_.logger.output = stdout;
        }
        if (mtx_init(&g_default_logger_context_.logger.mutex, mtx_plain)
            != thrd_success) {
            fprintf(stderr, "Fatal: Failed to initialize default logging mutex\n");
            abort();
        }
    }
}

// Get current logger context
LoggerContext *logger_get_context(void) {
    call_once(&g_logger_context_init_flag, initialize_g_logger_context_once);
    return g_logger_context_;
}

// Set logger context for current thread
LoggerContext *logger_set_context(LoggerContext *ctx) {
    assert(ctx != NULL);

    call_once(&g_logger_context_init_flag, initialize_g_logger_context_once);
    LoggerContext *previous = g_logger_context_;
    g_logger_context_       = ctx;
    return previous;
}

// Initialize a logger context
void logger_init_context(LoggerContext *ctx, char const *name,
                         char const *log_file_path) {
    assert(name != NULL);

    // Initialize the logger structure
    ctx->logger.enabled      = true;
    ctx->logger.min_level    = LOG_INFO;
    ctx->logger.output       = NULL;
    ctx->logger.close_output = false;
    ctx->name                = name; // Assuming name has static lifetime

    // Initialize mutex
    if (mtx_init(&ctx->logger.mutex, mtx_plain) != thrd_success) {
        ctx->logger.enabled = false;
        return;
    }

    // Set up output file if provided
    if (log_file_path != NULL) {
        FILE *file;
#ifdef __STDC_LIB_EXT1__
        constraint_handler_t previous_handler
            = set_constraint_handler_s(logger_constraint_handler);
#endif
        errno_t error = fopen_s(&file, log_file_path, "a+");
        if (error != 0) {
            char buf[256] = {0};
            strerror_s(buf, sizeof buf, error);
            fprintf(stderr,
                    "Warning: failed to open log file %s for suite %s (error: %s); "
                    "using stdout\n",
                    log_file_path, name, buf);
            file = stdout;
        } else {
            ctx->logger.close_output = true;
        }
#ifdef __STDC_LIB_EXT1__
        set_constraint_handler_s(previous_handler);
#endif
        ctx->logger.output = file;
    } else {
        ctx->logger.output = stdout;
    }
}

// Clean up a logger context
void logger_cleanup_context(LoggerContext *ctx) {
    if (ctx == NULL) {
        return;
    }

    if (ctx->logger.close_output && ctx->logger.output != NULL) {
        fclose(ctx->logger.output);
        ctx->logger.output = NULL;
    }

    // Destroy the mutex
    mtx_destroy(&ctx->logger.mutex);
}

char const *logger_get_filename(char const *path) {
    char const *last_slash     = strrchr(path, '/');
    char const *last_backslash = strrchr(path, '\\');

    // Find the rightmost separator
    char const *last_sep = last_slash;
    if (last_backslash > last_sep) {
        last_sep = last_backslash;
    }

    return last_sep ? last_sep + 1 : path;
}

// Initialize logger (call once at startup)
void logger_init(void) {
    call_once(&g_logger_context_init_flag, initialize_g_logger_context_once);
}

static void logger_constraint_handler(char const *restrict msg, void *restrict ptr,
                                      errno_t error) {
    BUT_UNUSED(ptr);
    fprintf(stderr, "%s: %d\n", msg ? msg : "unknown error", error);
}

// Cleanup logger (call at shutdown)
void logger_cleanup(void) {
    LoggerContext *ctx    = logger_get_context();
    Logger        *logger = &ctx->logger;

    mtx_destroy(&logger->mutex);
    if (logger->close_output) {
        fclose(logger->output);
        logger->output = NULL;
    }
}

// Configuration functions
void logger_enable(void) {
    LoggerContext *ctx  = logger_get_context();
    ctx->logger.enabled = true;
}

void logger_disable(void) {
    LoggerContext *ctx  = logger_get_context();
    ctx->logger.enabled = false;
}

void logger_set_level(log_level_t level) {
    LoggerContext *ctx    = logger_get_context();
    ctx->logger.min_level = level;
}

void logger_set_output(FILE *file) {
    LoggerContext *ctx = logger_get_context();
    ctx->logger.output = file ? file : stdout;
}

// open a file in append mode
void logger_set_output_by_filename(char const *path) {
    FILE          *file;
    LoggerContext *ctx    = logger_get_context();
    Logger        *logger = &ctx->logger;
#ifdef __STDC_LIB_EXT1__
    constraint_handler_t previous_handler
        = set_constraint_handler_s(logger_constraint_handler);
#endif
    errno_t error = fopen_s(&file, path, "a+");
    if (error != 0) {
        char buf[256] = {0};
        strerror_s(buf, sizeof buf, error);
        fprintf(stderr, "Error: failed to open %s (error: %s); falling back to stdout\n",
                path, buf);
        file = stdout;
    } else {
        logger->close_output = true;
    }
#ifdef __STDC_LIB_EXT1__
    set_constraint_handler_s(previous_handler);
#endif

    logger->output = file;
}

void logger_close() {
    LoggerContext *ctx    = logger_get_context();
    Logger        *logger = &ctx->logger;
    if (logger->close_output && logger->output != NULL) {
        fclose(logger->output);
        logger->output       = NULL;
        logger->close_output = false;
    }
}

static unsigned long get_thread_id(void) {
    if (my_thread_id == 0) {
        my_thread_id = atomic_fetch_add(&next_thread_id, 1);
    }
    return my_thread_id;
}

// Format timestamp
static void format_timestamp(char *buffer, size_t size) {
    time_t     now     = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Core logging function
void log_write(log_level_t level, char const *file, int line, char const *function,
               char const *test_name, char const *format, ...) {
    LoggerContext *ctx    = logger_get_context();
    Logger        *logger = &ctx->logger;
    // Quick exit if logging disabled or level too low
    if (!logger->enabled || level > logger->min_level) {
        return;
    }

    // Thread-safe logging
    mtx_lock(&logger->mutex);

    // Format timestamp
    char timestamp[32];
    format_timestamp(timestamp, sizeof(timestamp));

    // Get thread ID
    unsigned long tid = get_thread_id();

    // Extract just the filename from full path
    char const *filename = logger_get_filename(file);

    // Print log header
    fprintf(logger->output, "[%s] [%s] [T:%lu] %s:%d %s()", timestamp,
            level_names[level], tid, filename, line, function);

    // Add test name if provided
    if (test_name && test_name[0] != '\0') {
        fprintf(logger->output, " [%s]", test_name);
    }

    fprintf(logger->output, " ");

    // Print the actual message
    va_list args;
    va_start(args, format);
    vfprintf(logger->output, format, args);
    va_end(args);

    fprintf(logger->output, "\n");
    fflush(logger->output);

    mtx_unlock(&logger->mutex);
}
