#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG
#  define DBG(M, ...) fprintf(stderr, "[DEBUG] (%s:%d) : " M "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#  define INFO(M, ...) fprintf(stderr, "[INFO] (%s:%d) : " M "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#  define WARN(M, ...) fprintf(stderr, "[WARNING] (%s:%d) : " M "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#  define ERROR(M, ...) fprintf(stderr, "[ERROR] (%s:%d) :" M "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#  define DBG(M, ...)
#  define INFO(M, ...) fprintf(stderr, "[INFO] " M "\n", ##__VA_ARGS__)
#  define WARN(M, ...) fprintf(stderr, "[WARNING] " M "\n", ##__VA_ARGS__)
#  define ERROR(M, ...) fprintf(stderr, "[ERROR] " M "\n", ##__VA_ARGS__)
#endif

#endif
