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
    int i = 0;


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

/*void cat_redirect(char **commands){

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
}*/

int launch_comand(char **commands) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {



        if(strcmp(commands[0],"ls") == 0 ){

            if (execl("./ls","ls",commands[1],commands[2],NULL) == -1) {
                perror("Execvp execution ERROR");
                exit(EXIT_FAILURE);
            }

        }

        if(strcmp(commands[0],"cat") == 0 ){

            if (execl("./cat","cat",commands[1],commands[2],commands[3],NULL) == -1) {
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
