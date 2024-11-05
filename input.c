#include "input.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>

void interpret(const char *inputExpression)
{
    setInput(inputExpression);
    ASTNode *program = parseProgram();
    if (DEBUG) printf("Interpreter: Parsed program successfully\n");
    evaluateAST(program);
    if (DEBUG) printf("Interpreter: Evaluated AST successfully\n");
    freeAST(program);
}

void interpretFile(const char *fileName)
{
    FILE *file = fopen(fileName, "r");
    if (file == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }
    char line[1024];
    char *inputBuffer = NULL;
    size_t inputSize = 0;
    while (fgets(line, sizeof(line), file))
    {
        size_t len = strlen(line);
        inputBuffer = realloc(inputBuffer, inputSize + len + 1);
        strcpy(inputBuffer + inputSize, line);
        inputSize += len;
    }
    fclose(file);

    interpret(inputBuffer);

    free(inputBuffer);
}

void interactiveMode()
{
    char inputLine[256];
    printf("Mode interactif. Tapez 'exit' pour quitter.\n");
    while (1)
    {
        printf("> ");
        if (fgets(inputLine, sizeof(inputLine), stdin) == NULL)
        {
            break;
        }
        inputLine[strcspn(inputLine, "\n")] = 0;
        if (strcmp(inputLine, "exit") == 0)
        {
            break;
        }
        interpret(inputLine);
    }
}

void handleInput()
{
    int mode = 0;

    do
    {
        printf("Choisissez le mode d'exécution:\n");
        printf("1. Mode fichier\n");
        printf("2. Mode interactif\n");
        printf("Entrez votre choix (1 ou 2): ");
        scanf("%d", &mode);
        getchar();
    } while (mode != 1 && mode != 2);

    if (mode == 1)
    {
        printf("Entrez le nom du fichier: ");
        char fileName[256];
        fgets(fileName, sizeof(fileName), stdin);
        fileName[strcspn(fileName, "\n")] = 0;
        interpretFile(fileName);
    }
    else if (mode == 2)
    {
        interactiveMode();
    }
}

// Fonction principale
int main()
{
    handleInput();
    return 0;
}