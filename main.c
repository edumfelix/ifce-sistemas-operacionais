#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define TRUE 1
#define COMMAND_SIZE 100
#define PARAMETER_SIZE 20

// VARIAVEIS GLOBAIS
char command[COMMAND_SIZE];
char *parameters[PARAMETER_SIZE];
int status;

// PROTOTIPOS
void type_prompt();
void read_command();
char *get_location(char *);

int main()
{
    while (TRUE) // repita para sempre
    {
        type_prompt(); // mostra prompt na tela
        read_command(); // le entrada do terminal

        // Cria um processo filho
        if (fork() != 0)
        {
            waitpid(-1, &status, 0); // aguarda o processo filho acabar
        }
        else
        {
            char *real_command = get_location(command);
            execve(real_command, parameters, 0); // executa o comando
        }
    }
    return 0;
}

void type_prompt()
{
    printf("admin$ ");
}

void read_command()
{
    char *token;
    int i = 0;
    fgets(command, COMMAND_SIZE, stdin);
    token = strtok(command, " \n");
    while (token != NULL) {
        parameters[i] = token;
        token = strtok(NULL, " \n");
        i++;
    }
    parameters[i] = NULL;
}

char *get_location(char *command)
{
    // Obtém o caminho do comando do ambiente
    char *path = getenv("PATH");
    struct stat buffer;
    if (path)
    {
        // Cria uma cópia do caminho
        char *path_copy = strdup(path);

        // Percorre o caminho, procurando o comando
        char *path_token = strtok(path_copy, ":");
        while (path_token != NULL)
        {
            // Concatena o diretório com o comando
            char *file_path = malloc(strlen(path_token) + strlen(command) + 2);
            strcpy(file_path, path_token);
            strcat(file_path, "/");
            strcat(file_path, command);

            // Verifica se o arquivo existe
            if (stat(file_path, &buffer) == 0)
            {
                // Libera a memória da cópia do caminho
                free(path_copy);

                // Retorna o caminho do arquivo
                return file_path;
            }
            else
            {
                // Libera a memória do caminho do arquivo
                free(file_path);
            }

            // Avança para o próximo token
            path_token = strtok(NULL, ":");
        }

        // Libera a memória da cópia do caminho
        free(path_copy);

        // Verifica se o comando existe no diretório atual
        if (stat(command, &buffer) == 0)
        {
            // Retorna o caminho do comando
            return command;
        }
    }

    // Retorna NULL se o comando não for encontrado
    return NULL;
}