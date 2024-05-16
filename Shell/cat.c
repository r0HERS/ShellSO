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

int cat(char **commands) {

    if (commands[1] == NULL) {
        fprintf(stderr, "\nNo argument, expected : cat <DOCUMENT>\n");
        return 1;
    }

    if(commands[2] != NULL){
        if(strcmp(commands[2],">") == 0){
            cat_redirect(commands);
        }
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
    printf("\n\n");
    fclose(file);

    return 1;
}


int main(int argc,char **commands){

    cat(commands);
    printf("\n\n");
return 0;
}
