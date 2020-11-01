//
// Created by hugo on 10/27/20.
//

#ifndef TP3_ULTRA_CP_H
#define TP3_ULTRA_CP_H

#include <stdlib.h>
#define MAX_BUFFER_LENGTH 1024
#define U_AFLAG 1u << 0u
#define U_FFLAG 1u << 1u

ssize_t ultra_cp(const char *source_files[], size_t src_len, const char *dest, unsigned int flags);

#endif//TP3_ULTRA_CP_H
