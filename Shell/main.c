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

    if (commands[1] == NULL) {
        printf("Current path directories:\n");
        for (int i = 0; i < num_path_dirs; i++) {
            printf("    %s\n", path_dirs[i]);
        }
    } else {
        for (int i = 1; commands[i] != NULL; i++) {
            if (num_path_dirs < MAX_PATH_DIRS - 1) {
                path_dirs[num_path_dirs++] = strdup(commands[i]);
            } else {
                fprintf(stderr, "Max path directories exceeded\n");
                break;
            }
        }
    }
    return 1;
}

/*char *find_executable(const char *commands, int start) {
    char *full_path = NULL;

    // Percorre todos os diretórios em path_dirs
    for (int i = 0; i < num_path_dirs; i++) {
        // Tamanho necessário para alocar espaço para o caminho completo
        size_t path_len = strlen(path_dirs[i]) + strlen(commands(start)) + 2;  // +2 para '/' e '\0'
        char *path_exec = (char *)malloc(path_len);

        if (path_exec == NULL) {
            perror("Memory allocation error");
            continue;
        }

        // Constrói o caminho completo para o executável
        snprintf(path_exec, path_len, "%s/%s", path_dirs[i], executable);

        // Verifica se o executável existe nesse caminho
        if (access(path_exec, X_OK) == 0) {
            // Executável encontrado, retorna o caminho completo
            full_path = path_exec;
            break;
        }

        // Liberar a memória alocada para o caminho temporário
        free(path_exec);
    }

    return full_path;
}*/


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

int launch_comand(char **commands,int start) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {

        if(strcmp(commands[start],"ls") == 0 ){

            if (execl("./ls","ls",commands[start+1],commands[start+2],NULL) == -1) {
                perror("Execvp execution ERROR");
                exit(EXIT_FAILURE);
            }

        }

        if(strcmp(commands[start],"cat") == 0 ){

            if (execl("./cat","cat",commands[start+1],commands[start+2],commands[start+3],NULL) == -1) {
                perror("Execvp execution ERROR");
                exit(EXIT_FAILURE);
            }

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

    int parallel = 0;
    int i = 0;
    int start = 0;
    int end = 0;

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

    if (parallel==1) {
        while (commands[end] != NULL) {
            if (strcmp(commands[end], "&") == 0) {
                commands[end] = NULL;
                pid_t pid = fork();

                if (pid == 0) {
                    for (int i = 0; i < num_builtins(); i++) {
                        if (strcmp(commands[start], builtin_str[i]) == 0) {
                            return (*builtin_fun[i])(commands);
                            }
                    }

                    launch_comand(commands,start);
                    exit(EXIT_FAILURE);
                } else if (pid < 0) {
                    perror("fork ERROR");
                }
                start = end + 1;
            }
            end++;
        }
        }

    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(commands[0], builtin_str[i]) == 0) {
            return (*builtin_fun[i])(commands);
        }
    }

    return launch_comand(commands,start);
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
        status = exec_comand(commands);

        free(input);
        free(commands);
    } while (status);

    return 0;
}
