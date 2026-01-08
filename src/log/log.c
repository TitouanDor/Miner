#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "log.h"

char file_name[99];

int init_log(void) {
    
    snprintf((char *)file_name, sizeof(file_name), ".log");
    FILE *log_file = fopen(file_name, "a");
    if (log_file == NULL) {
        printf("Failed to create log file.\n");
        return 1;
    }
    time_t now = time(NULL);
    fprintf(log_file, "\n\n[INFO] : Log started at %s", ctime(&now));
    fclose(log_file);
    return 0;
}

int write_log(int type, const char *message, ...) {
    FILE *log_file = fopen(file_name, "a");
    if (log_file == NULL) {
        return 1;
    }

    switch (type) {
    case LOG_INFO:
        fprintf(log_file, "[INFO] : ");
        break;
    case LOG_WARNING:
        fprintf(log_file, "[WARNING] : ");
        break;
    case LOG_ERROR:
        fprintf(log_file, "[ERROR] : ");
        break;
    
    default:
        break;
    }

    va_list args;
    va_start(args, message);
    vfprintf(log_file, message, args);
    va_end(args);
    fprintf(log_file, "\n");
    fclose(log_file);
    return 0;
}
