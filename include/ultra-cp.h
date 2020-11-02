//
// Created by hugo on 10/27/20.
//

#ifndef TP3_ULTRA_CP_H
#define TP3_ULTRA_CP_H

#include <stdlib.h>
#include <sys/stat.h>

#define MAX_BUFFER_LENGTH 1024
#define U_AFLAG 1u << 0u
#define U_FFLAG 1u << 1u

/**
 * Copy files from sources_files to the destination
 * @param source_files path of files to be copied
 * @param src_len number of files to be copied
 * @param dest the path of destination
 * @param flags flags which program should be run with. See U_*FLAG
 * @return the size of the last file copied, or -1 if an error occurred
 */
ssize_t ultra_cp(const char *source_files[], size_t src_len, const char *dest, unsigned int flags);

/**
 * A simple wrapper to copy file using the system call sendfile
 * @param out_file the destination where the file must be copied
 * @param in_file the source file to copy
 * @param in_sb a pointer to the input file stat buffer
 * @return size of copied data; -1 if an error occurred
 */
ssize_t copy_file_wrapper(const char *out_file, const char *in_file, const struct stat *in_sb);

#endif//TP3_ULTRA_CP_H
