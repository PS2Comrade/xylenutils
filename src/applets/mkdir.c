#include "../xylenutils.h"
#include <sys/stat.h>

static int parents = 0;

static int mkdir_p(const char *path) {
    char *path_copy = strdup(path);
    char *p = path_copy;

    if (*p == '/') p++;

    while (*p) {
        while (*p && *p != '/') p++;

        if (*p == '/') {
            *p = '\0';
            if (mkdir(path_copy, 0755) < 0 && errno != EEXIST) {
                xu_error("mkdir: cannot create directory '%s'", path_copy);
                free(path_copy);
                return XU_ERROR;
            }
            *p = '/';
            p++;
        }
    }

    if (mkdir(path_copy, 0755) < 0 && errno != EEXIST) {
        xu_error("mkdir: cannot create directory '%s'", path_copy);
        free(path_copy);
        return XU_ERROR;
    }

    free(path_copy);
    return XU_SUCCESS;
}

int mkdir_main(int argc, char **argv) {
    int i;

    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        char *opt = &argv[i][1];
        while (*opt) {
            switch (*opt) {
                case 'p':
                    parents = 1;
                    break;
                default:
                    fprintf(stderr, "mkdir: invalid option -- '%c'\n", *opt);
                    return XU_ERROR;
            }
            opt++;
        }
    }

    if (i >= argc) {
        fprintf(stderr, "mkdir: missing operand\n");
        return XU_ERROR;
    }

    int ret = XU_SUCCESS;
    for (; i < argc; i++) {
        if (parents) {
            if (mkdir_p(argv[i]) != XU_SUCCESS) {
                ret = XU_ERROR;
            }
        } else {
            if (mkdir(argv[i], 0755) < 0) {
                xu_error("mkdir: cannot create directory '%s'", argv[i]);
                ret = XU_ERROR;
            }
        }
    }

    return ret;
}
