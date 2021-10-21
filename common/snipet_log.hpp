#pragma once

#include <stdio.h>

namespace snipet{
#define LOG_error(...) \
    do { \
        fprintf(stderr, "[Error]: %s:%d\n\t", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        exit(-1); \
    } while(0)

#define LOG_info(...) \
    do { \
        fprintf(stderr, "[Info]: %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
    } while(0)

#define LOG_warn(...) \
    do { \
        fprintf(stderr, "[Warning]: %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
    } while(0)
}

#define LOG(level, ...) LOG_ ##level(__VA_ARGS__) 