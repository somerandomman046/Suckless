#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#define WALL_DIR "/home/comic/.config/wallpapers/"
#define MAX_FILES 1024

// this script provided by pixel2175

int main() {
    DIR *d;
    struct dirent *dir;
    char *files[MAX_FILES];
    int count = 0;

    d = opendir(WALL_DIR);
    if (!d) {
        perror("opendir");
        return 1;
    }

    while ((dir = readdir(d)) != NULL && count < MAX_FILES) {
        if (dir->d_type == DT_REG) {
            files[count] = strdup(dir->d_name);
            count++;
        }
    }
    closedir(d);

    if (count == 0) {
        fprintf(stderr, "No files found.\n");
        return 1;
    }

    srand(time(NULL));
    int index = rand() % count;

    char command[1024];

    snprintf(command, sizeof(command), "walrs -i  %s%s   -q", WALL_DIR, files[index]);
    system(command);

    system("/home/$USER/.config/startup/color_scheme");

    // Free allocated memory
    for (int i = 0; i < count; i++) {
        free(files[i]);
    }

    return 0;
}
