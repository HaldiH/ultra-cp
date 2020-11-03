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

/**
 * Get the octal based string permission representation
 * @param m 3 bits corresponding to octal base
 * @return string of corresponding permissions with rwx order, with '-' for 0
 */
const char *mode(unsigned char m);

/**
 * Print the file properties in stdout [mode, size, modification date, filename]
 * @param sb a pointer to the file stat buffer
 * @param file path of the file from which to print properties
 * @return 0 if all succeed, -1 if an error occurred
 */
int print_file(const struct stat *sb, const char *file);

#endif//TP3_ULTRA_LS_H
