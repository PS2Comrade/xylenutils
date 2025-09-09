#include "../xylenutils.h"
#include <sys/stat.h>
#include <dirent.h>

static int recursive = 0;

static int cp_file_to_file(const char *src, const char *dst) {
    if (xu_copy_file(src, dst) < 0) {
        xu_error("cp: cannot copy '%s' to '%s'", src, dst);
        return XU_ERROR;
    }
    return XU_SUCCESS;
}

static int cp_file_to_dir(const char *src, const char *dst_dir) {
    char dst_path[1024];
    char *src_basename = strrchr(src, '/');
    src_basename = src_basename ? src_basename + 1 : (char *)src;

    snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, src_basename);
    return cp_file_to_file(src, dst_path);
}

static int cp_dir_recursive(const char *src, const char *dst) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char src_path[1024], dst_path[1024];

    if (mkdir(dst, 0755) < 0 && errno != EEXIST) {
        xu_error("cp: cannot create directory '%s'", dst);
        return XU_ERROR;
    }

    dir = opendir(src);
    if (!dir) {
        xu_error("cp: cannot open directory '%s'", src);
        return XU_ERROR;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst, entry->d_name);

        if (stat(src_path, &st) < 0) {
            xu_error("cp: cannot stat '%s'", src_path);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            if (cp_dir_recursive(src_path, dst_path) != XU_SUCCESS) {
                closedir(dir);
                return XU_ERROR;
            }
        } else {
            if (cp_file_to_file(src_path, dst_path) != XU_SUCCESS) {
                closedir(dir);
                return XU_ERROR;
            }
        }
    }

    closedir(dir);
    return XU_SUCCESS;
}

int cp_main(int argc, char **argv) {
    int i;

    // Parse options
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        char *opt = &argv[i][1];
        while (*opt) {
            switch (*opt) {
                case 'r':
                case 'R':
                    recursive = 1;
                    break;
                default:
                    fprintf(stderr, "cp: invalid option -- '%c'\n", *opt);
                    return XU_ERROR;
            }
            opt++;
        }
    }

    if (argc - i < 2) {
        fprintf(stderr, "cp: missing file operand\n");
        return XU_ERROR;
    }

    char *dst = argv[argc - 1];
    int is_dst_dir = xu_is_directory(dst);

    // Multiple sources require destination to be a directory
    if (argc - i > 2 && !is_dst_dir) {
        fprintf(stderr, "cp: target '%s' is not a directory\n", dst);
        return XU_ERROR;
    }

    int ret = XU_SUCCESS;
    for (; i < argc - 1; i++) {
        char *src = argv[i];

        if (xu_is_directory(src)) {
            if (!recursive) {
                fprintf(stderr, "cp: -r not specified; omitting directory '%s'\n", src);
                ret = XU_ERROR;
                continue;
            }

            if (is_dst_dir) {
                char dst_path[1024];
                char *src_basename = strrchr(src, '/');
                src_basename = src_basename ? src_basename + 1 : src;
                snprintf(dst_path, sizeof(dst_path), "%s/%s", dst, src_basename);
                if (cp_dir_recursive(src, dst_path) != XU_SUCCESS) {
                    ret = XU_ERROR;
                }
            } else {
                if (cp_dir_recursive(src, dst) != XU_SUCCESS) {
                    ret = XU_ERROR;
                }
            }
        } else {
            if (is_dst_dir) {
                if (cp_file_to_dir(src, dst) != XU_SUCCESS) {
                    ret = XU_ERROR;
                }
            } else {
                if (cp_file_to_file(src, dst) != XU_SUCCESS) {
                    ret = XU_ERROR;
                }
            }
        }
    }

    return ret;
}
