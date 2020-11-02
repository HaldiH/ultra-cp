//
// Created by hugo on 10/27/20.
//

#include "ultra-cp.h"
#include "ultra-ls.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

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
    struct stat out_sb;
    int rc = stat(out_file, &out_sb);

    // If the flag '-f' is enabled
    if (flags & U_FFLAG && in_sb->st_mode & S_IFLNK) {
        char sym_link[MAX_BUFFER_LENGTH] = {};
        // Read the string pointing to original file, stored in the symbolic link
        if (readlink(in_file, sym_link, MAX_BUFFER_LENGTH) < 0) {
            fprintf(stderr, "Cannot read symbolic link '%s'.\n", in_file);
            return -1;
        }

        // Get the absolute path to avoid breaking link
        char *r_path;
        if (!(r_path = realpath(sym_link, NULL))) {
            printf("%s\n", sym_link);
            perror("realpath");
            return -1;
        }

        // Create the new symlink
        if (symlink(r_path, out_file) < 0) {
            perror("symlink");
            return -1;
        }
        return 0;
    }

    // If the flag '-a' has been triggered
    if (rc >= 0 && flags & U_AFLAG)
        chmod(out_file, in_sb->st_mode);

    // If the output is a directory, we need to copy recursively each file to a new directory
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
        }

        DIR *dir = opendir(in_file);
        struct dirent *ent;
        // iterate each file in directory
        while ((ent = readdir(dir))) {
            if (strncmp(ent->d_name, ".", MAX_BUFFER_LENGTH - 1) == 0 ||
                strncmp(ent->d_name, "..", MAX_BUFFER_LENGTH - 1) == 0)
                continue;

            // Add trailing '/' after the directory name
            char in[MAX_BUFFER_LENGTH];
            char out[MAX_BUFFER_LENGTH];
            strncpy(in, in_file, MAX_BUFFER_LENGTH);
            strncat(in, "/", MAX_BUFFER_LENGTH - 1);
            strncat(in, ent->d_name, MAX_BUFFER_LENGTH);

            strncpy(out, out_dir, MAX_BUFFER_LENGTH);
            strncat(out, "/", MAX_BUFFER_LENGTH - 1);
            strncat(out, ent->d_name, MAX_BUFFER_LENGTH);

            struct stat next_sb;
            rc = flags & U_FFLAG ? lstat(in, &next_sb) : stat(in, &next_sb);
            if (rc < 0)
                return -1;

            if ((rc = copy(out, in, &next_sb)) < 0) {
                return -1;
            }
        }
        return rc;
    }

    // Copy the file only if the source is more recent or if the size aren't equal
    if (rc >= 0 &&
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

    // If more than 1 file has been given
    if (src_len > 1) {
        // We create the destination as a folder if doesn't exist
        if (rc < 0) {
            if (mkdir(dest, 0755) < 0) {
                fprintf(stderr, "Error: '%s' is not a valid path.\n", dest);
                return -1;
            }
        } else if (!(out_sb.st_mode & (S_IFDIR))) {
            // We throw an error if the destination isn't a folder
            fprintf(stderr, "Error: '%s' is not a valid path.\n", dest);
            return -1;
        }
    }

    for (int i = 0; i < src_len; i++) {
        const char *src_file = source_files[i];
        struct stat src_sb;

        char out[MAX_BUFFER_LENGTH];
        strncpy(out, dest, MAX_BUFFER_LENGTH);

        // If we have more than one source file, we need to add a trailing '/' after the directory name
        if (src_len > 1 ||
            out_sb.st_mode & S_IFDIR) {
            strncat(out, "/", MAX_BUFFER_LENGTH - 1);
            char b_name[MAX_BUFFER_LENGTH];
            strncpy(b_name, src_file, MAX_BUFFER_LENGTH);
            basename(b_name);
            strncat(out, b_name, MAX_BUFFER_LENGTH);
        }

        rc = flags & U_FFLAG ? lstat(src_file, &src_sb) : stat(src_file, &src_sb);

        if (rc < 0) {
            fprintf(stderr, "Error: cannot access '%s'.\n", src_file);
            continue;
        }

        if (copy(out, src_file, &src_sb) < 0) {
            return -1;
        }
    }

    return 0;
}
