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

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "path"
};

int (*builtin_fun[])(char **) = {
    &cd,
    &help,
    &shell_exit,
    &shell_path
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
        fprintf(stderr, "\nSem argumento, esperado: \"cd <folder> \n");
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
    fflush(stdin);
    fflush(stdout);
    return 0;
}

int shell_path(char **commands) {
    if (commands[1] == NULL) {
        printf("Paths atuais:\n");
        for (int i = 0; i < num_path_dirs; i++) {
            printf("    %s\n", path_dirs[i]);
        }
    } else {
        for (int i = 1; commands[i] != NULL; i++) {
            if (num_path_dirs < MAX_PATH_DIRS - 1) {
                path_dirs[num_path_dirs++] = strdup(commands[i]);
            } else {
                fprintf(stderr, "Maximo numero de paths excedido\n");
                break;
            }
        }
    }
    return 1;
}

char *find_executable(const char *executable) {
    char *full_path = NULL;

    for (int i = 0; i < num_path_dirs; i++) {
        size_t path_len = strlen(path_dirs[i]) + strlen(executable) + 2;  // +2 for '/' and '\0'
        char *path_exec = (char *)malloc(path_len);

        if (path_exec == NULL) {
            perror("Alocaçao de  memoria ERROR");
            continue;
        }

        snprintf(path_exec, path_len, "%s/%s", path_dirs[i], executable);

        printf("\nBuscando em: %s\n\n", path_exec);

        if (access(path_exec, X_OK) == 0) {
            full_path = path_exec;
            break;
        }

        free(path_exec);
    }

    return full_path;
}

void *read_input() {
    int BUFFER_SIZE = LSH_RL_BUFFER;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
    int c;

    if (!buffer) {
        fprintf(stderr, "Alocaçao do buffer ERROR\n");
        return NULL;
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
            BUFFER_SIZE += LSH_RL_BUFFER;
            buffer = realloc(buffer, BUFFER_SIZE);
            if (!buffer) {
                fprintf(stderr, "Realocaçao de buffer ERROR\n");
                return NULL;
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
        fprintf(stderr, "Tokens alocaçao ERROR\n");
        return NULL;
    }

    token = strtok(input, LSH_TOK_DELIM);

    while (token != NULL) {
        tokens[pos] = token;
        pos++;

        if (pos >= BUFFER_SIZE) {
            BUFFER_SIZE += LSH_TOK_BUFFERSIZE;
            tokens = realloc(tokens, BUFFER_SIZE * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "Tokens realocaçao ERROR\n");
                return NULL;
            }
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[pos] = NULL;
    return tokens;
}

int launch_command(char **commands, int start) {
    char *executable = find_executable(commands[start]);

    if (executable == NULL) {
        fprintf(stderr, "Executavel não encontrado\n");
        return 1;
    }

    if (execl(executable, commands[start], commands[start + 1], commands[start + 2], commands[start + 3], NULL) == -1) {
        perror("Execl exucuçao ERROR");
        return 1;
    }

    free(executable);
    return 1;
}

int exec_command(char **commands) {
    int parallel = 0;
    int i = 0;

    if (commands[0] == NULL) {
        return 1;
    }

    while (commands[i] != NULL) {
        if (strcmp(commands[i], "&") == 0) {
            parallel = 1;
            break;
        }
        i++;
    }

    if (parallel == 1) {
        int start = 0;
        for (int j = 0; commands[j] != NULL; j++) {
            if (strcmp(commands[j], "&") == 0) {
                commands[j] = NULL;
                pid_t pid = fork();

                if (pid == 0) {
                    for (int k = 0; k < num_builtins(); k++) {
                        if (strcmp(commands[start], builtin_str[k]) == 0) {
                            exit((*builtin_fun[k])(commands + start));
                        }
                    }
                    launch_command(commands, start);
                    return 1;
                } else if (pid < 0) {
                    perror("fork ERROR");
                }
                start = j + 1;
            }
        }

        if (commands[start] != NULL && commands[start][0] != '\0') {
            pid_t pid = fork();

            if (pid == 0) {
                for (int k = 0; k < num_builtins(); k++) {
                    if (strcmp(commands[start], builtin_str[k]) == 0) {
                        return (*builtin_fun[i])(commands + start);
                    }
                }
                launch_command(commands, start);
                return 1;
            } else if (pid < 0) {
                perror("fork ERROR");
            }
        }

        while (wait(NULL) > 0);
    } else {
        for (int i = 0; i < num_builtins(); i++) {
            if (strcmp(commands[0], builtin_str[i]) == 0) {
                return (*builtin_fun[i])(commands);
            }
        }

        pid_t pid = fork();
        if (pid == 0) {
            launch_command(commands, 0);
            return 1;
        } else if (pid < 0) {
            perror("Fork ERROR");
        } else {
            wait(NULL);
        }
    }

    return 1;
}

void text() {
    char cwd[1024];

    getcwd(cwd, sizeof(cwd));
    printf("\n%s > ", cwd);
}

int main(int argc, char **argv) {
    char *input;
    char **commands;
    int status;

    do {
        text();
        input = read_input();
        commands = split_input(input);
        status = exec_command(commands);

        free(input);
        free(commands);
    } while (status);

    return 0;
}
