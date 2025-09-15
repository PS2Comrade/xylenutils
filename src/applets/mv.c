#include "../xylenutils.h"
#include <sys/stat.h>

static int verbose = 0;

static int mv_file(const char *src, const char *dst) {
    struct stat src_st, dst_st;
    
    if (stat(src, &src_st) < 0) {
        xu_error("mv: cannot stat '%s'", src);
        return XU_ERROR;
    }
    
    // Check if destination exists and is a directory
    if (stat(dst, &dst_st) == 0 && S_ISDIR(dst_st.st_mode)) {
        char *basename = strrchr(src, '/');
        basename = basename ? basename + 1 : (char *)src;
        
        char new_dst[1024];
        snprintf(new_dst, sizeof(new_dst), "%s/%s", dst, basename);
        return mv_file(src, new_dst);
    }
    
    // Try rename first (fastest)
    if (rename(src, dst) == 0) {
        if (verbose)
            printf("'%s' -> '%s'\n", src, dst);
        return XU_SUCCESS;
    }
    
    // If rename failed, try copy + delete
    if (xu_copy_file(src, dst) < 0) {
        xu_error("mv: cannot copy '%s' to '%s'", src, dst);
        return XU_ERROR;
    }
    
    if (unlink(src) < 0) {
        xu_error("mv: cannot remove '%s'", src);
        return XU_ERROR;
    }
    
    if (verbose)
        printf("'%s' -> '%s'\n", src, dst);
        
    return XU_SUCCESS;
}

int mv_main(int argc, char **argv) {
    int i;
    
    xu_check_help(argc, argv, "mv [-v] source dest");
    
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        char *opt = &argv[i][1];
        while (*opt) {
            switch (*opt) {
                case 'v': verbose = 1; break;
                default:
                    fprintf(stderr, "mv: invalid option -- '%c'\n", *opt);
                    return XU_ERROR;
            }
            opt++;
        }
    }
    
    if (argc - i < 2) {
        fprintf(stderr, "mv: missing operand\n");
        return XU_ERROR;
    }
    
    char *dst = argv[argc - 1];
    int ret = XU_SUCCESS;
    
    for (; i < argc - 1; i++) {
        if (mv_file(argv[i], dst) != XU_SUCCESS)
            ret = XU_ERROR;
    }
    
    return ret;
}
