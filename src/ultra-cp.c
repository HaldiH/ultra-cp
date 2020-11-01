//
// Created by hugo on 10/27/20.
//

#include "ultra-cp.h"
#include "ultra-ls.h"
#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

unsigned int flags = 0;

ssize_t copy_file_wrapper(const char *out_file, const char *in_file, const struct stat *in_sb) {
    int in_fd;
    if ((in_fd = open(in_file, 0)) < 0) {
        fprintf(stderr, "Error: cannot open '%s'.\n", in_file);
        return -1;
    }

    int out_fd;
    if ((out_fd = open(out_file, O_CREAT | O_WRONLY, in_sb->st_mode)) < 0) {
        fprintf(stderr, "Error: cannot create file '%s'.\n", out_file);
        return -1;
    }

    int rc;
    if ((rc = sendfile(out_fd, in_fd, 0, in_sb->st_size)) < 0) {
        fprintf(stderr, "Error: cannot copy file '%s'.\n", in_file);
        return -1;
    }
    return rc;
}

ssize_t copy(const char *out_file, const char *in_file, const struct stat *in_sb) {
    if (in_sb->st_mode & S_IFDIR) {
        char out_dir[MAX_BUFFER_LENGTH];
        strncpy(out_dir, out_file, MAX_BUFFER_LENGTH);

        struct stat out_sb;
        int rc = stat(out_dir, &out_sb);

        if (rc < 0) {
            if (mkdir(out_dir, in_sb->st_mode) < 0) {
                fprintf(stderr, "Error: '%s' is not a valid path.\n", out_dir);
                return -1;
            }
        } else if (out_sb.st_mode & S_IFDIR) {
            strncat(out_dir, "/", MAX_BUFFER_LENGTH);
            strncat(out_dir, basename((char *) in_file), MAX_BUFFER_LENGTH);
            mkdir(out_dir, in_sb->st_mode);
        }
        DIR *dir = opendir(in_file);
        struct dirent *ent;
        while ((ent = readdir(dir))) {
            if (strncmp(ent->d_name, ".", MAX_BUFFER_LENGTH - 1) == 0 ||
                strncmp(ent->d_name, "..", MAX_BUFFER_LENGTH - 1) == 0)
                continue;

            char in[MAX_BUFFER_LENGTH];
            char out[MAX_BUFFER_LENGTH];
            strncpy(in, in_file, MAX_BUFFER_LENGTH);
            strncat(in, "/", MAX_BUFFER_LENGTH);
            strncat(in, ent->d_name, MAX_BUFFER_LENGTH);

            strncpy(out, out_dir, MAX_BUFFER_LENGTH);
            strncat(out, "/", MAX_BUFFER_LENGTH);
            strncat(out, ent->d_name, MAX_BUFFER_LENGTH);

            struct stat next_sb;
            if ((stat(in, &next_sb)) < 0)
                return -1;

            if ((rc = copy(out, in, &next_sb)) < 0) {
                return -1;
            }
        }
        return rc;
    }

    struct stat out_sb;

    if (stat(out_file, &out_sb) >= 0 &&
        difftime(out_sb.st_mtim.tv_sec, in_sb->st_mtim.tv_sec) >= 0 &&
        out_sb.st_size == in_sb->st_size)
        return 0;

    return copy_file_wrapper(out_file, in_file, in_sb);
}

ssize_t ultra_cp(const char *source_files[], size_t src_len, const char *dest, unsigned int _flags) {
    // Should never happen
    if (src_len < 1)
        return -1;
    flags = _flags;

    struct stat out_sb;
    int rc = stat(dest, &out_sb);

    if (src_len > 1) {
        if (rc < 0) {
            if (mkdir(dest, 0755) < 0) {
                fprintf(stderr, "Error: '%s' is not a valid path.\n", dest);
                return -1;
            }
        } else if (!(out_sb.st_mode & (S_IFDIR | S_IFBLK))) {
            fprintf(stderr, "Error: '%s' is not a valid path.\n", dest);
            return -1;
        }
    }

    for (int i = 0; i < src_len; i++) {
        const char *src_file = source_files[i];
        struct stat src_sb;

        char out[MAX_BUFFER_LENGTH];
        strncpy(out, dest, MAX_BUFFER_LENGTH);

        if (src_len > 1 ||
            out_sb.st_mode & S_IFDIR) {
            strncat(out, "/", MAX_BUFFER_LENGTH);
            strncat(out, basename((char *) src_file), MAX_BUFFER_LENGTH);
        }


        if (stat(src_file, &src_sb) < 0) {
            fprintf(stderr, "Error: cannot access '%s'.\n", src_file);
            continue;
        }

        if (copy(out, src_file, &src_sb) < 0) {
            return -1;
        }
    }

    return 0;
}
