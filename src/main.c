#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include "xylenutils.h"

typedef int (*applet_main_t)(int argc, char **argv);

typedef struct {
    const char *name;
    applet_main_t main;
    const char *help;
} applet_t;

// Forward declarations
int ls_main(int argc, char **argv);
int cat_main(int argc, char **argv);
int cp_main(int argc, char **argv);
int echo_main(int argc, char **argv);
int pwd_main(int argc, char **argv);
int mkdir_main(int argc, char **argv);
int rm_main(int argc, char **argv);
int mv_main(int argc, char **argv);
int touch_main(int argc, char **argv);
int head_main(int argc, char **argv);
int tail_main(int argc, char **argv);

static const applet_t applets[] = {
    {"cat", cat_main, "concatenate files to stdout"},
    {"cp", cp_main, "copy files or directories"},
    {"echo", echo_main, "display a line of text"},
    {"head", head_main, "output first part of files"},
    {"ls", ls_main, "list directory contents"},
    {"mkdir", mkdir_main, "make directories"},
    {"mv", mv_main, "move/rename files"},
    {"pwd", pwd_main, "print working directory"},
    {"rm", rm_main, "remove files and directories"},
    {"tail", tail_main, "output last part of files"},
    {"touch", touch_main, "change file timestamps"},
    {NULL, NULL, NULL}
};

static void show_usage(const char *progname) {
    fprintf(stderr, "Usage: %s [applet] [arguments...]\n\n", progname);
    fprintf(stderr, "Available applets:\n");
    
    for (const applet_t *applet = applets; applet->name != NULL; applet++) {
        fprintf(stderr, "  %-10s %s\n", applet->name, applet->help);
    }
}

int main(int argc, char **argv) {
    char *progname = basename(argv[0]);
    const char *applet_name = progname;
    
    if (strcmp(progname, "xylenutils") == 0) {
        if (argc < 2) {
            show_usage(progname);
            return 1;
        }
        applet_name = argv[1];
        argv++;
        argc--;
    }
    
    for (const applet_t *applet = applets; applet->name != NULL; applet++) {
        if (strcmp(applet_name, applet->name) == 0) {
            return applet->main(argc, argv);
        }
    }
    
    fprintf(stderr, "%s: applet not found\n", applet_name);
    show_usage(progname);
    return 1;
}
