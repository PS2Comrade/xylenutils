#include "../xylenutils.h"
#include <fcntl.h>
#include <sys/stat.h>

static int lines = 10;

static int tail_file(const char *filename) {
    int fd;
    struct stat st;
    char *buffer;
    off_t file_size;
    ssize_t bytes_read;
    
    if (strcmp(filename, "-") == 0) {
        // For stdin, we need to buffer everything
        fd = STDIN_FILENO;
        buffer = malloc(1024 * 1024);  // 1MB buffer
        if (!buffer) return XU_ERROR;
        
        bytes_read = read(fd, buffer, 1024 * 1024);
        if (bytes_read < 0) {
            free(buffer);
            xu_error("tail: read error");
            return XU_ERROR;
        }
        file_size = bytes_read;
    } else {
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            xu_error("tail: %s", filename);
            return XU_ERROR;
        }
        
        if (fstat(fd, &st) < 0) {
            xu_error("tail: %s", filename);
            close(fd);
            return XU_ERROR;
        }
        
        file_size = st.st_size;
        buffer = malloc(file_size);
        if (!buffer) {
            close(fd);
            return XU_ERROR;
        }
        
        bytes_read = read(fd, buffer, file_size);
        if (bytes_read != file_size) {
            free(buffer);
            close(fd);
            xu_error("tail: read error");
            return XU_ERROR;
        }
    }
    
    // Count lines from the end
    int line_count = 0;
    off_t pos = file_size - 1;
    
    while (pos >= 0 && line_count < lines) {
        if (buffer[pos] == '\n') {
            line_count++;
        }
        if (line_count < lines) pos--;
    }
    
    // Print from pos+1 to end
    if (pos < file_size - 1) {
        write(STDOUT_FILENO, buffer + pos + 1, file_size - pos - 1);
    }
    
    free(buffer);
    if (fd != STDIN_FILENO) close(fd);
    return XU_SUCCESS;
}

int tail_main(int argc, char **argv) {
    int i;
    
    xu_check_help(argc, argv, "tail [-n lines] [file...]");
    
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        if (strncmp(argv[i], "-n", 2) == 0) {
            if (argv[i][2] != '\0') {
                lines = atoi(&argv[i][2]);
            } else if (i + 1 < argc) {
                lines = atoi(argv[++i]);
            } else {
                fprintf(stderr, "tail: option requires an argument -- n\n");
                return XU_ERROR;
            }
        } else {
            fprintf(stderr, "tail: invalid option -- '%c'\n", argv[i][1]);
            return XU_ERROR;
        }
    }
    
    if (i >= argc) {
        return tail_file("-");
    }
    
    int ret = XU_SUCCESS;
    int file_count = argc - i;
    
    for (; i < argc; i++) {
        if (file_count > 1) {
            printf("==> %s <==\n", argv[i]);
        }
        
        if (tail_file(argv[i]) != XU_SUCCESS) {
            ret = XU_ERROR;
        }
        
        if (file_count > 1 && i < argc - 1) {
            printf("\n");
        }
    }
    
    return ret;
}
