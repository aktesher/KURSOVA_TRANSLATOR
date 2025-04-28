#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Header.h"

// таблиця лексем
Token* TokenTable;
// кількість лексем
unsigned int TokensNum;
// таблиця ідентифікаторів
Id* IdTable;
// кількість ідентифікаторів
unsigned int IdNum;

int main(int argc, char* argv[])
{
    // таблиця лексем
    TokenTable = new Token[MAX_TOKENS];
    // таблиця ідентифікаторів
    IdTable = new Id[MAX_IDENTIFIER];
    char InputFile[32] = "";
    FILE* InFile;

    if (argc != 2)
    {
        printf("Input file name: ");
        gets_s(InputFile);
    }
    else
    {
        strcpy_s(InputFile, argv[1]);
    }

    if ((fopen_s(&InFile, InputFile, "rt")) != 0)
    {
        printf("Error: Can not open file: %s\n", InputFile);
        return 1;
    }

    char NameFile[32] = "";
    int i = 0;
    while (InputFile[i] != '.' && InputFile[i] != '\0')
    {
        NameFile[i] = InputFile[i];
        i++;
    }
    NameFile[i] = '\0';

    char TokenFile[32];
    strcpy_s(TokenFile, NameFile);
    strcat_s(TokenFile, ".token");

    // генерація файлу з помилками
    FILE* logFile;
    if (fopen_s(&logFile, "errorlog.txt", "w") != 0)
    {
        printf("Error: Can not open log file: errorlog.txt\n");
        return 1;
    }

    // лексичний аналіз
    TokensNum = GetTokens(InFile, TokenTable, logFile);

    PrintTokensToFile(TokenFile, TokenTable, TokensNum);
    fclose(InFile);
    printf("\nLexical analysis completed. List of tokens in the file %s\n", TokenFile);
    PrintTokens(TokenTable, TokensNum);

    // синтаксичний аналіз
    Parser( logFile);

    //72 line MN// генерація вихідного С коду
    char OutputFile[32];
    strcpy_s(OutputFile, NameFile);
    strcat_s(OutputFile, ".cpp");

    FILE* outFile;
    fopen_s(&outFile, OutputFile, "w");
    if (!outFile)
    {
        printf("Failed to open output file.\n");
        exit(1);
    }
    //84 line MN//
    generateCCode(outFile);
    printf("\nC++ code has been generated and written to %s.\n \n", OutputFile);
    // Закрытие файла логов после использования
    fclose(logFile);
    _fcloseall();
    delete[] TokenTable;
    delete[] IdTable;

    return 0;
}
