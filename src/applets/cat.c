#include "../xylenutils.h"
#include <fcntl.h>

static int cat_file(const char *filename) {
    int fd;
    char buffer[8192];
    ssize_t bytes_read;
    
    if (strcmp(filename, "-") == 0) {
        fd = STDIN_FILENO;
    } else {
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            xu_error("cat: %s", filename);
            return XU_ERROR;
        }
    }
    
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
            xu_error("cat: write error");
            if (fd != STDIN_FILENO) close(fd);
            return XU_ERROR;
        }
    }
    
    if (bytes_read < 0) {
        xu_error("cat: read error");
        if (fd != STDIN_FILENO) close(fd);
        return XU_ERROR;
    }
    
    if (fd != STDIN_FILENO) {
        close(fd);
    }
    
    return XU_SUCCESS;
}

int cat_main(int argc, char **argv) {
    int ret = XU_SUCCESS;
    
    xu_check_help(argc, argv, "cat [file...]");
    
    if (argc == 1) {
        return cat_file("-");
    }
    
    for (int i = 1; i < argc; i++) {
        if (cat_file(argv[i]) != XU_SUCCESS) {
            ret = XU_ERROR;
        }
    }
    
    return ret;
}
