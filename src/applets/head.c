#include "../xylenutils.h"
#include <fcntl.h>

static int lines = 10;  // default -n 10

static int head_file(const char *filename) {
    int fd;
    char buffer[8192];
    ssize_t bytes_read;
    int line_count = 0;
    
    if (strcmp(filename, "-") == 0) {
        fd = STDIN_FILENO;
    } else {
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            xu_error("head: %s", filename);
            return XU_ERROR;
        }
    }
    
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (write(STDOUT_FILENO, &buffer[i], 1) != 1) {
                xu_error("head: write error");
                if (fd != STDIN_FILENO) close(fd);
                return XU_ERROR;
            }
            
            if (buffer[i] == '\n') {
                line_count++;
                if (line_count >= lines) {
                    if (fd != STDIN_FILENO) close(fd);
                    return XU_SUCCESS;
                }
            }
        }
    }
    
    if (fd != STDIN_FILENO) close(fd);
    return XU_SUCCESS;
}

int head_main(int argc, char **argv) {
    int i;
    
    xu_check_help(argc, argv, "head [-n lines] [file...]");
    
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        if (strncmp(argv[i], "-n", 2) == 0) {
            if (argv[i][2] != '\0') {
                lines = atoi(&argv[i][2]);
            } else if (i + 1 < argc) {
                lines = atoi(argv[++i]);
            } else {
                fprintf(stderr, "head: option requires an argument -- n\n");
                return XU_ERROR;
            }
        } else {
            fprintf(stderr, "head: invalid option -- '%c'\n", argv[i][1]);
            return XU_ERROR;
        }
    }
    
    if (i >= argc) {
        return head_file("-");
    }
    
    int ret = XU_SUCCESS;
    int file_count = argc - i;
    
    for (; i < argc; i++) {
        if (file_count > 1) {
            printf("==> %s <==\n", argv[i]);
        }
        
        if (head_file(argv[i]) != XU_SUCCESS) {
            ret = XU_ERROR;
        }
        
        if (file_count > 1 && i < argc - 1) {
            printf("\n");
        }
    }
    
    return ret;
}
