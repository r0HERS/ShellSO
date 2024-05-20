# Projeto Shell SO

Bem-vindo ao Projeto Shell SO, desenvolvido para a disciplina de Sistemas Operacionais. Este projeto é um interpretador de comandos básico que inclui funções embutidas como `cd`, `help`, `exit`, e `path`. Além disso, o projeto também suporta comandos externos como `ls` e `cat`.

## Autores

Este trabalho foi desenvolvido pelo grupo 8:
- Renan Rohers
- Tiago Dallécio
- Kauai Duhamel
- Luis Guilherme

## Funcionalidades

- `cd <pasta>`: Muda o diretório atual para `<pasta>`.
- `help`: Exibe a ajuda com uma lista de comandos embutidos.
- `exit`: Sai do shell.
- `path <caminho>`: Define os caminhos para busca de executáveis.
- Comandos externos como `ls` e `cat`.

## Pré-requisitos

Para compilar e executar este projeto, você precisará de:
- Um sistema operacional Linux.
- Compilador GCC.
- Acesso à linha de comando.

## Instruções de Compilação e Execução

Siga os passos abaixo para compilar e executar o projeto:

1. **Clone o repositório**:
    ```sh
    git clone "https://github.com/r0HERS/ShellSO.git"
    cd "ShellSO"
    ```

2. **Compile o código-fonte**:
    ```sh
    gcc -c main.c
    gcc -o main main.o
    ```
    ```sh
    gcc -c ls.c
    gcc -o ls ls.o
    ```
    ```sh
    gcc -c cat.c
    gcc -o cat cat.o
    ```

3. **Execute o shell**:
    ```sh
    ./main
    ```

## Detalhes da Implementação

### Estrutura do Código

- `main.c`: Arquivo principal do projeto que contém a lógica do shell.
- `ls.c`: Implementação do comando `ls`.
- `cat.c`: Implementação do comando `cat`.

### Funções Embutidas

#### `cd`

```c
int cd(char **commands);
Muda o diretório atual para o especificado em commands[1].

help

int help(char **commands);
Exibe a lista de comandos embutidos e informações sobre o projeto.

exit

int shell_exit(char **commands);
Sai do shell.

path

int shell_path(char **commands);
Define ou exibe os caminhos para busca de executáveis.

Funções Externas

ls e derivados
O comando ls lista o conteúdo de um diretório. Está implementado no arquivo ls.c.

cat e derivados
O comando cat exibe o conteúdo de arquivos. Está implementado no arquivo cat.c.

Agradecimentos
Agradecemos ao professor Isaías Bittencourt Felzmann de Sistemas Operacionais por fornecer a base e o apoio necessários para a realização deste projeto.
