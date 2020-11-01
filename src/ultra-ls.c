//
// Created by hugo on 10/27/20.
//

#include "ultra-ls.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char *mode(unsigned long m) {
    static char mode[4];
    mode[0] = m >> 2 & 1 ? 'r' : '-';
    mode[1] = m >> 1 & 1 ? 'w' : '-';
    mode[2] = m & 1 ? 'x' : '-';
    return mode;
}

int print_file(const struct stat *sb, const char *file) {
    switch (sb->st_mode & S_IFMT) {
        case S_IFDIR:
            printf("d");
            break;
        case S_IFREG:
            printf("-");
            break;
        case S_IFLNK:
            printf("l");
            break;
        case S_IFBLK:
            printf("b");
            break;
        case S_IFCHR:
            printf("c");
            break;
        case S_IFSOCK:
            printf("s");
            break;
        default:
            printf("?");
            break;
    }
    printf("%s", mode(sb->st_mode >> 6));
    printf("%s", mode(sb->st_mode >> 3));
    printf("%s", mode(sb->st_mode));

    printf("%10lu", sb->st_size);
    char time[MAX_BUFFER_LENGTH];
    strftime(time, MAX_BUFFER_LENGTH, "%c", localtime((const time_t *) &sb->st_mtim));
    printf(" %s   %s\n", time, file);
    return 0;
}

int is_trailing_char(const char *s, char c, unsigned int length) {
    if (s[0] == '\0')
        return 0;
    for (unsigned int i = 0; i < length; i++) {
        if (s[i + 1] == '\0') {
            if (s[i] == c)
                return 1;
            return 0;
        }
    }
    return 0;
}

int ultra_ls(const char *file) {
    struct stat sb;

    if (lstat(file, &sb)) {
        fprintf(stderr, "Error: cannot access '%s'\n", file);
        return EXIT_FAILURE;
    }

    if ((sb.st_mode & S_IFMT) == S_IFDIR) {
        DIR *d;
        if (!(d = opendir(file))) {
            fprintf(stderr, "Error: cannot access directory '%s'\n", file);
            return EXIT_FAILURE;
        }
        print_file(&sb, file);

        struct dirent *dir;
        while ((dir = readdir(d))) {
            if (strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0)
                continue;

            char file_path[MAX_BUFFER_LENGTH];
            strncpy(file_path, file, MAX_BUFFER_LENGTH);

            if (!is_trailing_char(file_path, '/', MAX_BUFFER_LENGTH))
                strcat(file_path, "/");
            strncat(file_path, dir->d_name, MAX_BUFFER_LENGTH);

            if (lstat(file_path, &sb)) {
                fprintf(stderr, "Error: cannot access '%s'\n", file_path);
                return EXIT_FAILURE;
            }
            print_file(&sb, file_path);
        }
        closedir(d);
        return 0;
    }
    print_file(&sb, file);
    return 0;
}
