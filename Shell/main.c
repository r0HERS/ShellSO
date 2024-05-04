#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define LSH_RL_BUFFER 1024

#define LSH_TOK_BUFFERSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

int cd(char **comands);
int help(char **comands);
int shell_exit(char **comands);

char *builtin_str[] = {

    "cd",
    "help",
    "exit"

};

int (*builtin_fun[])(char ** ) = {

    &cd,
    &help,
    &shell_exit

};



int num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}

int cd(char **comands){

    if(comands[1] == NULL){
        fprintf(stderr, "\nNo argumnet, expected : \"cd\"\n");
    }else{

    if(chdir(comands[1]) != 0){
        perror("cd ERROR");
    }

    }
    return 1;
}

int help(char **comands){

    int i=0;

    printf("Programa Shell SO\nFunctions:\n");

    for(i=0;i< num_builtins();i++){
        printf("    - %s\n", builtin_str[i]);
    }
    return 1;
}

int shell_exit(char **comands){

    return 0;
}

void *read_input(){

    int BUFFER_SIZE = LSH_RL_BUFFER;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
    int c;

    if(!buffer){
        fprintf(stderr,"Buffer allocation space ERROR\n");
        exit(EXIT_FAILURE);
    }




    while(1){

        c = getchar();

        if(c == '\n'){
            buffer[pos] = '\0';
            return buffer;
        }else{
        buffer[pos] = c;
        }
        pos++;

        if (pos >= BUFFER_SIZE){
            BUFFER_SIZE = BUFFER_SIZE + LSH_RL_BUFFER;
            buffer = realloc(buffer, BUFFER_SIZE);
            if(!buffer){
                fprintf(stderr,"Buffer reallocation space ERROR\n");
                exit(EXIT_FAILURE);
            }
        }

    }
}


char **split_input(char *input)
{
    int BUFFER_SIZE = LSH_TOK_BUFFERSIZE, pos = 0;
    char **tokens = malloc(BUFFER_SIZE * sizeof(char*));
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
            tokens = realloc(tokens, BUFFER_SIZE * sizeof(char*));
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


int launch_comand(char **comands){

    pid_t pid,wpid;
    int status;

    pid = fork();

    if(pid == 0){

        if(execvp(comands[0],comands) == -1){
            perror("Execvp execution ERROR");
        }
        exit(EXIT_FAILURE);

    }else if(pid < 0){
        perror("Fork failed");

    }else{

        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


int exec_comand(char **comands){

        int i=0;

        if(comands[0] == NULL){

            return 1;
        }

        for(i=0;i<num_builtins();i++){
            if(strcmp(comands[0],builtin_str[i]) == 0){
                return (*builtin_fun[i])(comands);
            }
        }
        return launch_comand(comands);
}

void text(){
    char cwd[1024];

    getcwd(cwd,sizeof(cwd));
    printf("%s > ", cwd);

}

int main(int argc, char **argv)
{
    char *input;
    char **comands;
    int status;


    do{

        text();
        input = read_input();
        comands = split_input(input);
        status = exec_comand(comands);

        free(input);
        free(comands);

    }while(status);


    return 0;
}
