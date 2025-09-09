#include "../xylenutils.h"
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

static int show_all = 0;
static int show_long = 0;

static void print_permissions(mode_t mode) {
    printf("%c", S_ISDIR(mode) ? 'd' : (S_ISLNK(mode) ? 'l' : '-'));
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (mode & S_IXOTH) ? 'x' : '-');
}

static void ls_file(const char *name, const char *path) {
    struct stat st;
    
    if (stat(path, &st) < 0) {
        xu_error("ls: %s", path);
        return;
    }
    
    if (show_long) {
        print_permissions(st.st_mode);
        printf(" %3ld", st.st_nlink);
        
        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        printf(" %s %s", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");
        
        printf(" %8ld", st.st_size);
        
        char timestr[64];
        struct tm *tm = localtime(&st.st_mtime);
        strftime(timestr, sizeof(timestr), "%b %d %H:%M", tm);
        printf(" %s", timestr);
    }
    
    printf(" %s\n", name);
}

static int ls_dir(const char *dirname) {
    DIR *dir;
    struct dirent *entry;
    char path[1024];
    
    dir = opendir(dirname ? dirname : ".");
    if (!dir) {
        xu_error("ls: %s", dirname ? dirname : ".");
        return XU_ERROR;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }
        
        if (dirname) {
            snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);
        } else {
            strcpy(path, entry->d_name);
        }
        
        ls_file(entry->d_name, path);
    }
    
    closedir(dir);
    return XU_SUCCESS;
}

int ls_main(int argc, char **argv) {
    int i;
    int dir_count = 0;
    
    xu_check_help(argc, argv, "ls [-al] [file...]");
    
    // Parse options
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        if (strcmp(argv[i], "--") == 0) {
            i++;
            break;
        }
        
        char *opt = &argv[i][1];
        if (*opt == '-') {
            // Long option that isn't help
            fprintf(stderr, "ls: unrecognized option '%s'\n", argv[i]);
            return XU_ERROR;
        }
        
        while (*opt) {
            switch (*opt) {
                case 'a':
                    show_all = 1;
                    break;
                case 'l':
                    show_long = 1;
                    break;
                default:
                    fprintf(stderr, "ls: invalid option -- '%c'\n", *opt);
                    fprintf(stderr, "Try 'ls --help' for more information.\n");
                    return XU_ERROR;
            }
            opt++;
        }
    }
    
    // Count directories
    for (int j = i; j < argc; j++) {
        dir_count++;
    }
    
    if (dir_count == 0) {
        return ls_dir(NULL);
    }
    
    int ret = XU_SUCCESS;
    for (; i < argc; i++) {
        if (dir_count > 1) {
            printf("%s:\n", argv[i]);
        }
        if (ls_dir(argv[i]) != XU_SUCCESS) {
            ret = XU_ERROR;
        }
        if (dir_count > 1 && i < argc - 1) {
            printf("\n");
        }
    }
    
    return ret;
}
