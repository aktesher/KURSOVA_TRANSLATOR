#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Header.h"

// таблиц€ лексем
Token* TokenTable;
// к≥льк≥сть лексем
unsigned int TokensNum;
// таблиц€ ≥дентиф≥катор≥в
Id* IdTable;
// к≥льк≥сть ≥дентиф≥катор≥в
unsigned int IdNum;

int main(int argc, char* argv[])
{
    // таблиц€ лексем
    TokenTable = new Token[MAX_TOKENS];
    // таблиц€ ≥дентиф≥катор≥в
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

    // генерац≥€ файлу з помилками
    FILE* logFile;
    if (fopen_s(&logFile, "errorlog.txt", "w") != 0)
    {
        printf("Error: Can not open log file: errorlog.txt\n");
        return 1;
    }

    // лексичний анал≥з
    TokensNum = GetTokens(InFile, TokenTable, logFile);

    PrintTokensToFile(TokenFile, TokenTable, TokensNum);
    fclose(InFile);
    printf("\nLexical analysis completed. List of tokens in the file %s\n", TokenFile);
    PrintTokens(TokenTable, TokensNum);

    // синтаксичний анал≥з
    Parser( logFile);

    // «акрытие файла логов после использовани€
    fclose(logFile);
    _fcloseall();
    delete[] TokenTable;
    delete[] IdTable;

    return 0;
}
