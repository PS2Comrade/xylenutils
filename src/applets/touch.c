#include "../xylenutils.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <utime.h>

static int no_create = 0;  // -c

int touch_main(int argc, char **argv) {
    int i;
    
    xu_check_help(argc, argv, "touch [-c] file...");
    
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        char *opt = &argv[i][1];
        while (*opt) {
            switch (*opt) {
                case 'c': no_create = 1; break;
                default:
                    fprintf(stderr, "touch: invalid option -- '%c'\n", *opt);
                    return XU_ERROR;
            }
            opt++;
        }
    }
    
    if (i >= argc) {
        fprintf(stderr, "touch: missing file operand\n");
        return XU_ERROR;
    }
    
    int ret = XU_SUCCESS;
    time_t now = time(NULL);
    
    for (; i < argc; i++) {
        struct stat st;
        
        if (stat(argv[i], &st) == 0) {
            // File exists, update timestamps
            struct utimbuf times = {now, now};
            if (utime(argv[i], &times) < 0) {
                xu_error("touch: cannot touch '%s'", argv[i]);
                ret = XU_ERROR;
            }
        } else {
            // File doesn't exist
            if (no_create) {
                continue;  // -c flag: don't create
            }
            
            int fd = open(argv[i], O_CREAT | O_WRONLY, 0644);
            if (fd < 0) {
                xu_error("touch: cannot touch '%s'", argv[i]);
                ret = XU_ERROR;
            } else {
                close(fd);
            }
        }
    }
    
    return ret;
}
