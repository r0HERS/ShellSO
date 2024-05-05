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


#define LSH_RL_BUFFER 1024
#define LSH_TOK_BUFFERSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define MAX_PATH_DIRS 256

char *path_dirs[MAX_PATH_DIRS];
int num_path_dirs = 0;

int cd(char **commands);
int help(char **commands);
int shell_exit(char **commands);
int shell_path(char **commands);
int ls(char **commands);
int cat(char **commands);
void cat_redirect(char **commands);

char *defpath();

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "path",
    "ls",
    "cat"
};

int (*builtin_fun[])(char **) = {
    &cd,
    &help,
    &shell_exit,
    &shell_path,
    &ls,
    &cat
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

void remove_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

int cd(char **commands) {
    if (commands[1] == NULL) {
        fprintf(stderr, "\nNo argument, expected : \"cd\"\n");
    } else {
        if (chdir(commands[1]) != 0) {
            int found = 0;
            for (int i = 0; i < num_path_dirs; i++) {
                char path[1024];
                snprintf(path, sizeof(path), "%s/%s", path_dirs[i], commands[1]);
                if (chdir(path) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                perror("cd ERROR");
            }
        }
    }
    return 1;
}

int help(char **commands) {
    printf("Programa Shell SO\nFunctions:\n");
    for (int i = 0; i < num_builtins(); i++) {
        printf("    - %s\n", builtin_str[i]);
    }
    return 1;
}

int shell_exit(char **commands) {
    return 0;
}

int shell_path(char **commands) {
    int i = 0;

    if (commands[1] == NULL) {
        char *stdpath = defpath();
        if (stdpath != NULL) {
            char *token = strtok(stdpath, ":");
            while (token != NULL && num_path_dirs < MAX_PATH_DIRS) {
                path_dirs[num_path_dirs++] = strdup(token);
                token = strtok(NULL, ":");
            }
        } else {
            perror("No path found");
        }
        for (i = 0; i < num_path_dirs; i++) {
            printf("%s\n", path_dirs[i]);
        }
    } else {
        num_path_dirs = 0;
        for (int i = 0; commands[i + 1] != NULL && i < MAX_PATH_DIRS; i++) {
            path_dirs[i] = strdup(commands[i + 1]);
            num_path_dirs++;
        }
        printf("New path:\n\n");
        for (i = 0; i < num_path_dirs; i++) {
            printf("%s\n", path_dirs[i]);
        }
    }
    return 1;
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
                printf(ANSI_COLOR_GREEN "  %s" ANSI_COLOR_RESET , entry->d_name);

        } else {
            perror("stat ERROR");
        }
    }
    printf("\n");
    closedir(dir);
    return 1;
}

int cat(char **commands) {

    if (commands[1] == NULL) {
        fprintf(stderr, "\nNo argument, expected : cat <DOCUMENT>");
    }

    if(strcmp(commands[2],">") == 0){
        cat_redirect(commands);
    }

    FILE *file = fopen(commands[1], "r");

    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", commands[1]);
        return 1;
    }

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    fclose(file);
    return 1;
}

void cat_redirect(char **commands){

    pid_t pid, wpid;
    int status;
    int redirect_output = 0;
    char *output_filename = NULL;

     redirect_output = 1;
     output_filename = commands[3];
     commands[2] = NULL;

     if (pid == 0) {
        if (redirect_output) {

            FILE *output_file = fopen(output_filename, "w");
            if (output_file == NULL) {
                perror("Erro ao abrir o arquivo de saída");
                exit(EXIT_FAILURE);
            }
            dup2(fileno(output_file), STDOUT_FILENO);
            fclose(output_file);
        }

        if (execvp(commands[0], commands) == -1) {
            perror("Erro na execução do comando");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("Erro no fork");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }



}

char *defpath() {
    char *stdpath = getenv("PATH");
    return stdpath;
}

void *read_input() {
    int BUFFER_SIZE = LSH_RL_BUFFER;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
    int c;

    if (!buffer) {
        fprintf(stderr, "Buffer allocation space ERROR\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();

        if (c == '\n') {
            buffer[pos] = '\0';
            return buffer;
        } else {
            buffer[pos] = c;
        }
        pos++;

        if (pos >= BUFFER_SIZE) {
            BUFFER_SIZE = BUFFER_SIZE + LSH_RL_BUFFER;
            buffer = realloc(buffer, BUFFER_SIZE);
            if (!buffer) {
                fprintf(stderr, "Buffer reallocation space ERROR\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **split_input(char *input) {
    int BUFFER_SIZE = LSH_TOK_BUFFERSIZE;
    int pos = 0;
    char **tokens = malloc(BUFFER_SIZE * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "Tokens allocation space ERROR\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(input, LSH_TOK_DELIM);

    while (token != NULL) {
        tokens[pos] = token;
        pos++;

        if (pos >= BUFFER_SIZE) {
            BUFFER_SIZE += LSH_TOK_BUFFERSIZE;
            tokens = realloc(tokens, BUFFER_SIZE * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "Tokens reallocation space ERROR\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[pos] = NULL;
    return tokens;
}

int launch_comand(char **commands) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {
        if (execvp(commands[0], commands) == -1) {
            perror("Execvp execution ERROR");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("Fork failed");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int exec_comand(char **commands) {
    if (commands[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(commands[0], builtin_str[i]) == 0) {
            return (*builtin_fun[i])(commands);
        }
    }

    return launch_comand(commands);
}

void text() {
    char cwd[1024];

    getcwd(cwd, sizeof(cwd));
    printf("%s > ", cwd);
}

int main(int argc, char **argv) {
    char *input;
    char **commands;
    int status;

    do {
        text();
        input = read_input();
        commands = split_input(input);
        status = exec_comand(commands);

        free(input);
        free(commands);
    } while (status);

    return 0;
}
