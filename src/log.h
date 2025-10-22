#ifndef LOG_H_
#define LOG_H_

/**
 * @file log.h
 * @author Douglas Cuthbertson
 * @brief
 * @version 0.2
 * @date 2025-07-17
 *
 * See LICENSE.txt for copyright and licensing information about this file.
 */
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#if defined(__cplusplus)
extern "C" {
#endif

// Convenience macros
#define LOG_FATAL(NAME, MSG, ...) \
    log_write(LOG_FATAL, __FILE__, __LINE__, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_ERROR(NAME, MSG, ...) \
    log_write(LOG_ERROR, __FILE__, __LINE__, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_WARN(NAME, MSG, ...) \
    log_write(LOG_WARN, __FILE__, __LINE__, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_INFO(NAME, MSG, ...) \
    log_write(LOG_INFO, __FILE__, __LINE__, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_VERBOSE(NAME, MSG, ...) \
    log_write(LOG_VERBOSE, __FILE__, __LINE__, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_DEBUG(NAME, MSG, ...) \
    log_write(LOG_DEBUG, __FILE__, __LINE__, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_TRACE(NAME, MSG, ...) \
    log_write(LOG_TRACE, __FILE__, __LINE__, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_FATAL_FILE_LINE(NAME, FILE, LINE, MSG, ...) \
    log_write(LOG_FATAL, FILE, LINE, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_ERROR_FILE_LINE(NAME, FILE, LINE, MSG, ...) \
    log_write(LOG_ERROR, FILE, LINE, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_WARN_FILE_LINE(NAME, FILE, LINE, MSG, ...) \
    log_write(LOG_WARN, FILE, LINE, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_INFO_FILE_LINE(NAME, FILE, LINE, MSG, ...) \
    log_write(LOG_INFO, FILE, LINE, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_VERBOSE_FILE_LINE(NAME, FILE, LINE, MSG, ...) \
    log_write(LOG_VERBOSE, FILE, LINE, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_DEBUG_FILE_LINE(NAME, FILE, LINE, MSG, ...) \
    log_write(LOG_DEBUG, FILE, LINE, __func__, (NAME), (MSG), ##__VA_ARGS__)
#define LOG_TRACE_FILE_LINE(NAME, FILE, LINE, MSG, ...) \
    log_write(LOG_TRACE, FILE, LINE, __func__, (NAME), (MSG), ##__VA_ARGS__)

// Log levels (hierarchical)
typedef enum {
    LOG_FATAL   = 0,
    LOG_ERROR   = 1,
    LOG_WARN    = 2,
    LOG_INFO    = 3,
    LOG_VERBOSE = 4,
    LOG_DEBUG   = 5,
    LOG_TRACE   = 6
} log_level_t;

// Logger configuration
typedef struct {
    bool        close_output;
    bool        enabled;
    log_level_t min_level;
    FILE       *output;
    mtx_t       mutex;
} Logger;

typedef struct {
    Logger      logger;
    char const *name; // For identification
} LoggerContext;

// Logger API
void logger_init(void);
void logger_cleanup(void);
void logger_enable(void);
void logger_set_level(log_level_t level);
void logger_set_output(FILE *file);
void log_write(log_level_t level, char const *file, int line, char const *function,
               char const *test_name, char const *format, ...);
char const *logger_get_filename(char const *path);

LoggerContext *logger_get_context(void);
LoggerContext *logger_set_context(LoggerContext *ctx);

// Create a logger context
void logger_init_context(LoggerContext *ctx, char const *name,
                         char const *log_file_path);
// Cleanup suite context
void logger_cleanup_context(LoggerContext *ctx);

#if defined(__cplusplus)
}
#endif

#endif /* LOG_H_ */
