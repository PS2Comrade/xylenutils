#include "../xylenutils.h"

int pwd_main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
    char *cwd = getcwd(NULL, 0);
    if (!cwd) {
        xu_error("pwd");
        return XU_ERROR;
    }

    printf("%s\n", cwd);
    free(cwd);
    return XU_SUCCESS;
}
