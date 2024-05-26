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
- Execução de arquivos batch que contém uma lista de comandos a serem executados sequencialmente.

## Pré-requisitos

Para compilar e executar este projeto, você precisará de:
- Um sistema operacional Linux.
- Compilador GCC.
- Acesso à linha de comando.
- Oracle VM VirtualBox com o Ubuntu Server 22.04.3 LTS instalado como máquina virtual.

## Configuração da Máquina Virtual e Pasta Compartilhada

### Parte I – Configuração e Instalação da Máquina Virtual

1. **Baixe a imagem do Ubuntu Server 22.04.3 LTS**:
   - [Download do Ubuntu Server](https://ubuntu.com/download/server)
   - Se estiver usando uma máquina do laboratório, acesse `\\hub\ACADEMICO\007-PROFs\Isaías-SO` pela rede interna para um download mais ágil.

2. **Instale o VirtualBox e o Extension Pack**:
   - [Download do VirtualBox](https://www.virtualbox.org/wiki/Downloads)

3. **Crie uma nova máquina virtual**:
   - Abra o VirtualBox e clique em "Novo".
   - Preencha o nome da VM e selecione a imagem ISO do Ubuntu baixada no passo 1.
   - Mantenha as configurações padrão para memória e armazenamento.

4. **Configuração de Rede**:
   - Vá em "Configurações" da VM > "Rede" > "Avançado" > "Redirecionamento de Portas".
   - Adicione uma nova regra com:
     - Endereço IP do Hospedeiro: 127.0.0.1
     - Porta do Hospedeiro: 2222
     - Porta do Convidado: 22

5. **Instalação do Ubuntu**:
   - Inicie a VM e siga as instruções para instalar o Ubuntu Server.
   - Selecione o idioma Português e continue com as configurações padrão.
   - Crie um usuário e senha conforme solicitado durante a instalação.

### Parte II – Configuração da Pasta Compartilhada

1. **Crie uma pasta compartilhada no sistema hospedeiro**:
   - No VirtualBox, vá em "Dispositivos" > "Pastas Compartilhadas" > "Configurações de Pastas Compartilhadas".
   - Adicione uma nova pasta compartilhada apontando para uma pasta no sistema hospedeiro (por exemplo, `C:\PUC-SO\Labs`).
   - Nomeie o compartilhamento como `labs`.

2. **Monte a pasta compartilhada na VM**:
   - No terminal do hospedeiro, conecte-se à VM via SSH:
     ```sh
     ssh <usuário>@127.0.0.1 -p 2222
     ```
   - Na VM, crie um diretório e monte a pasta compartilhada:
     ```sh
     mkdir labs
     sudo mount -t vboxsf labs labs/
     ```

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

### Execução de Arquivos Batch

Para executar um arquivo batch contendo uma lista de comandos:

1. **Crie ou edite seu arquivo batch**:
    - Certifique-se de que o arquivo batch esteja no mesmo diretório que o executável `main`.
    - O arquivo batch deve conter uma lista de comandos, um por linha.

    Exemplo de um arquivo batch (`comandos_batch.batch`):
    ```sh
    path /bin /usr/bin
    cd /home
    ls -l
    cat /etc/passwd
    ```

2. **Execute o arquivo batch**:
    ```sh
    ./main comandos_batch.batch
    ```

## Detalhes da Implementação

### Estrutura do Código

- `main.c`: Arquivo principal do projeto que contém a lógica do shell.
- `ls.c`: Implementação do comando `ls`.
- `cat.c`: Implementação do comando `cat`.

### Funções Embutidas

```c
cd

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
```

Agradecimentos
Agradecemos ao professor Isaías Bittencourt Felzmann de Sistemas Operacionais por fornecer a base e o apoio necessários para a realização deste projeto.
