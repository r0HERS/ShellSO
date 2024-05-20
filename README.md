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
    git clone <URL_DO_REPOSITORIO>
    cd <NOME_DO_REPOSITORIO>
    ```

2. **Compile o código-fonte**:
    ```sh
    gcc -o shell main.c ls.c cat.c
    ```

3. **Execute o shell**:
    ```sh
    ./shell
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

ls
O comando ls lista o conteúdo de um diretório. Está implementado no arquivo ls.c.

cat
O comando cat exibe o conteúdo de arquivos. Está implementado no arquivo cat.c.

Configuração de Localidade
O projeto está configurado para suportar caracteres acentuados. No início da função main, a localidade é configurada para UTF-8:

setlocale(LC_ALL, "en_US.UTF-8");
Como Contribuir
Se você deseja contribuir para este projeto, siga os passos abaixo:

Fork o repositório.
Crie um branch para suas modificações (git checkout -b minha-nova-funcionalidade).
Commit suas mudanças (git commit -am 'Adiciona nova funcionalidade').
Push para o branch (git push origin minha-nova-funcionalidade).
Crie um novo Pull Request.
Licença
Este projeto está licenciado sob a licença MIT. Veja o arquivo LICENSE para mais detalhes.

Agradecimentos
Agradecemos ao professor da disciplina de Sistemas Operacionais por fornecer a base e o apoio necessários para a realização deste projeto.

Substitua `<URL_DO_REPOSITORIO>` pelo URL real do seu repositório no GitHub e `<NOME_DO_REPOSITORIO
