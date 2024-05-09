#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>


#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void remove_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}


int ls(char **commands) {
    int show_hidden = 0;
    int long_format = 0;
    char *directory = ".";

    for (int i = 1; commands[i] != NULL; i++) {
        if (strcmp(commands[i], "-a") == 0) {
            show_hidden = 1;
        } else if (strcmp(commands[i], "-l") == 0) {
            long_format = 1;
        } else {
            directory = commands[i];
        }
    }


    DIR *dir = opendir(directory);
    if (!dir) {
        perror("ls ERROR");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

        struct stat st;
        if (stat(path, &st) == 0) {
            if (long_format) {
                printf((S_ISDIR(st.st_mode)) ? "d" : "-");
                printf((st.st_mode & S_IRUSR) ? "r" : "-");
                printf((st.st_mode & S_IWUSR) ? "w" : "-");
                printf((st.st_mode & S_IXUSR) ? "x" : "-");
                printf((st.st_mode & S_IRGRP) ? "r" : "-");
                printf((st.st_mode & S_IWGRP) ? "w" : "-");
                printf((st.st_mode & S_IXGRP) ? "x" : "-");
                printf((st.st_mode & S_IROTH) ? "r" : "-");
                printf((st.st_mode & S_IWOTH) ? "w" : "-");
                printf((st.st_mode & S_IXOTH) ? "x" : "-");
                printf(" %ld", (long)st.st_nlink);
                printf(" %s %s", getpwuid(st.st_uid)->pw_name, getgrgid(st.st_gid)->gr_name);
                printf(" %8ld", (long)st.st_size);

                char *mod_time_str = ctime(&st.st_mtime);
                remove_newline(mod_time_str);
                printf(" %s", mod_time_str);

            }
                printf(ANSI_COLOR_GREEN "  %s\n" ANSI_COLOR_RESET , entry->d_name);

        } else {
            perror("stat ERROR");
        }
    }
    printf("\n");
    closedir(dir);
    return 1;
}


int main(int argc,char **commands){

    ls(commands);

return 1;
}
