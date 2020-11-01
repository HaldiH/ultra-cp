//
// Created by hugo on 10/27/20.
//

#ifndef TP3_ULTRA_LS_H
#define TP3_ULTRA_LS_H

#include <sys/stat.h>

#define MAX_BUFFER_LENGTH 1024

int ultra_ls(const char *file);
const char *mode(unsigned long m);
int is_trailing_char(const char *s, char c, unsigned int length);
int print_file(const struct stat *sb, const char *file);

#endif//TP3_ULTRA_LS_H
