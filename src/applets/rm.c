#include "../xylenutils.h"
#include <sys/stat.h>
#include <dirent.h>

static int force = 0;      // -f
static int recursive = 0;  // -r
static int verbose = 0;    // -v

static int rm_file(const char *path) {
    struct stat st;
    
    if (lstat(path, &st) < 0) {
        if (!force) {
            xu_error("rm: cannot stat '%s'", path);
            return XU_ERROR;
        }
        return XU_SUCCESS;
    }
    
    if (S_ISDIR(st.st_mode)) {
        if (!recursive) {
            xu_error("rm: cannot remove '%s': Is a directory", path);
            return XU_ERROR;
        }
        
        DIR *dir = opendir(path);
        if (!dir) {
            if (!force) {
                xu_error("rm: cannot open directory '%s'", path);
                return XU_ERROR;
            }
            return XU_SUCCESS;
        }
        
        struct dirent *entry;
        char subpath[1024];
        int ret = XU_SUCCESS;
        
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
                
            snprintf(subpath, sizeof(subpath), "%s/%s", path, entry->d_name);
            if (rm_file(subpath) != XU_SUCCESS)
                ret = XU_ERROR;
        }
        closedir(dir);
        
        if (rmdir(path) < 0 && !force) {
            xu_error("rm: cannot remove directory '%s'", path);
            return XU_ERROR;
        }
        
        if (verbose)
            printf("removed directory '%s'\n", path);
            
        return ret;
    } else {
        if (unlink(path) < 0) {
            if (!force) {
                xu_error("rm: cannot remove '%s'", path);
                return XU_ERROR;
            }
        } else if (verbose) {
            printf("removed '%s'\n", path);
        }
    }
    
    return XU_SUCCESS;
}

int rm_main(int argc, char **argv) {
    int i;
    
    xu_check_help(argc, argv, "rm [-frv] file...");
    
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        char *opt = &argv[i][1];
        while (*opt) {
            switch (*opt) {
                case 'f': force = 1; break;
                case 'r':
                case 'R': recursive = 1; break;
                case 'v': verbose = 1; break;
                default:
                    fprintf(stderr, "rm: invalid option -- '%c'\n", *opt);
                    return XU_ERROR;
            }
            opt++;
        }
    }
    
    if (i >= argc) {
        fprintf(stderr, "rm: missing operand\n");
        return XU_ERROR;
    }
    
    int ret = XU_SUCCESS;
    for (; i < argc; i++) {
        if (rm_file(argv[i]) != XU_SUCCESS)
            ret = XU_ERROR;
    }
    
    return ret;
}
