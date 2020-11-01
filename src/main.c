#include "ultra-cp.h"
#include "ultra-ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(const char *s) {
    fprintf(stderr, "Usage: %s [-t] [-n] file [further_files] [destination]\n", s);
}

int main(int argc, char *argv[]) {
    int opt;
    int a_flag = 0,
        f_flag = 0;

    while ((opt = getopt(argc, argv, "fa")) != -1) {
        switch (opt) {
            case 'f':
                f_flag = 1;
                break;
            case 'a':
                a_flag = 1;
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (argc - optind < 1) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (argc - optind == 1)
        return ultra_ls(argv[optind]);

    size_t src_len = argc - optind - 1;
    char *src_files[src_len];
    for (int i = optind; i < argc; i++) {
        src_files[i - optind] = argv[i];
    }

    unsigned int flags = 0;
    if (a_flag)
        flags |= U_AFLAG;
    if (f_flag)
        flags |= U_FFLAG;

    return ultra_cp((const char **) src_files, src_len, argv[argc - 1], flags) < 0 ? 1 : 0;
}
