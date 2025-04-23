#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#define WALL_DIR "/home/comic/.config/wallpapers"
#define MAX_FILES 4096

static int valid_file(const char *name) { return name[0] != '.'; }

static char *random_wall(void)
{
    DIR *d = opendir(WALL_DIR);
    if (!d) { perror("opendir"); return NULL; }

    char *list[MAX_FILES];
    size_t n = 0;
    struct dirent *e;

    while ((e = readdir(d)) && n < MAX_FILES)
        if (valid_file(e->d_name))
            list[n++] = strdup(e->d_name);
    closedir(d);

    if (!n) {
        fprintf(stderr, "No wallpapers in %s\n", WALL_DIR);
        return NULL;
    }

    srand((unsigned)time(NULL));
    char *pick = list[rand() % n];
    char *path = malloc(PATH_MAX);
    snprintf(path, PATH_MAX, "%s/%s", WALL_DIR, pick);

    for (size_t i = 0; i < n; ++i) free(list[i]);
    return path;
}

static int run_walrs(const char *path)
{
    char cmd[PATH_MAX * 2];
    snprintf(cmd, sizeof cmd, "walrs -q -i \"%s\"", path);
    return system(cmd);
}

static char *first_line(const char *cmd)
{
    FILE *fp = popen(cmd, "r");
    if (!fp) return NULL;

    char *line = NULL;
    size_t n = 0;
    if (getline(&line, &n, fp) <= 0) { free(line); line = NULL; }
    if (line) line[strcspn(line, "\n")] = '\0';
    pclose(fp);
    return line;
}

int main(void)
{
    char sxiv_cmd[PATH_MAX * 2];
    snprintf(
        sxiv_cmd, sizeof sxiv_cmd,
        "sh -c 'printf \"./.\\n\"; LC_ALL=C ls -1A \"%s\"' | sxiv -otb -",
        WALL_DIR);

    char *sel = first_line(sxiv_cmd);
    if (!sel || !*sel) { free(sel); return 0; }

    char *target;
    if (strcmp(sel, "./.") == 0) {
        free(sel);
        target = random_wall();
        if (!target) return 1;
    } else {
        target = malloc(PATH_MAX);
        snprintf(target, PATH_MAX, "%s/%s", WALL_DIR, sel);
        free(sel);
    }

    int ok = run_walrs(target);
    free(target);
    return ok;
}
