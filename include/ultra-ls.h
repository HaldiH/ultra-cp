//
// Created by hugo on 10/27/20.
//

#ifndef TP3_ULTRA_LS_H
#define TP3_ULTRA_LS_H

#include <sys/stat.h>

#define MAX_BUFFER_LENGTH 1024

/**
 * List the file properties in stdout if it is a file, or list each file property if it is a directory
 * @param file the path to list file to
 * @return -1 if an error occurred
 */
int ultra_ls(const char *file);
const char *mode(unsigned long m);
int print_file(const struct stat *sb, const char *file);

#endif//TP3_ULTRA_LS_H
