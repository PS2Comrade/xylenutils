#include "xylenutils.h"
#include <sys/stat.h>
#include <fcntl.h>

void xu_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    if (errno) {
        fprintf(stderr, ": %s", strerror(errno));
    }
    fprintf(stderr, "\n");
}

int xu_copy_file(const char *src, const char *dst) {
    int src_fd, dst_fd;
    char buffer[8192];
    ssize_t bytes_read, bytes_written;
    struct stat st;
    
    src_fd = open(src, O_RDONLY);
    if (src_fd < 0) {
        return -1;
    }
    
    if (fstat(src_fd, &st) < 0) {
        close(src_fd);
        return -1;
    }
    
    dst_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode & 0777);
    if (dst_fd < 0) {
        close(src_fd);
        return -1;
    }
    
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dst_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            close(src_fd);
            close(dst_fd);
            return -1;
        }
    }
    
    close(src_fd);
    close(dst_fd);
    return bytes_read < 0 ? -1 : 0;
}

int xu_is_directory(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

// Check for help flags and print usage if found
int xu_check_help(int argc, char **argv, const char *usage) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s\n", usage);
            exit(XU_SUCCESS);
        }
    }
    return 0;
}
