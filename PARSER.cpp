#include <iostream>
#include "Header.h"

// таблиця лексем
extern Token* TokenTable;
// кількість лексем
extern unsigned int TokensNum;
// таблиця ідентифікаторів
extern Id* IdTable;
extern // кількість ідентифікаторів
unsigned int IdNum;

int pos = 0;
char matrix[100][8]; // Сохраняем до 100 меток, каждая до 7 символов + \0
unsigned int matrixSize = 0;

void Parser(FILE* logFile)
{
    program(logFile);
    printf("\nThe program is syntax correct.\n");
    printf("\n%d identifier found\n", IdIdentification(IdTable, TokenTable, TokensNum, logFile));
    fprintf(logFile, "No Syntax Errors\n");
   // fprintf(logFile, "\n%d identifier found.\n", IdIdentification(IdTable, TokenTable, TokensNum, logFile));
}
void match(TypeOfTokens expectedType, FILE* logFile)
{
    if (TokenTable[pos].type == expectedType)
    {
        pos++;
    }
    else
    {
        printf("\nSyntax error in line %d : another type of lexeme was expected.\n", TokenTable[pos].line);
        printf("\nSyntax error in line %d: Expected %d, but found :\n", TokenTable[pos].line, expectedType);
        fprintf(logFile, "\nSyntax error in line %d : another type of lexeme was expected.\n", TokenTable[pos].line);
        fprintf(logFile, "\nSyntax error in line %d: Expected %d, but found :\n", TokenTable[pos].line, expectedType);
        fprintf(logFile, "Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        exit(1);
    }
}
// <програма> ::= '#Program'<ідентифікатор>';' 'Start' 'Variable' <оголошення_змінних> ';' <тіло_програми>  'Stop' 
void program(FILE* logFile)
{
    match(ProgramName, logFile);    // Новый токен для начала программы
    match(Identifier, logFile);          // Имя программы
    match(Semicolon, logFile);         // Точка с запятой
    match(StartProgram, logFile);     // Начало основной части программы
    match(Variable, logFile);
    variable_declaration(logFile);
    match(Semicolon, logFile);
    program_body(logFile);
    match(StopProgram, logFile);
}
// <оголошення змінних> ::= [<тип даних> <список змінних>]
void variable_declaration(FILE* logFile)
{
    if (TokenTable[pos].type == Type)
    {
        pos++;
        variable_list(logFile);
    }
}
// <список змінних> ::= <ідентифікатор> { ',' <ідентифікатор> }
void variable_list(FILE* logFile)
{
    match(Identifier, logFile);
    while (TokenTable[pos].type == Comma)
    {
        pos++;
        match(Identifier, logFile);
    }
}
// <тіло програми> ::= <оператор> ';' { <оператор> ';' }
void program_body(FILE* logFile)
{
    do
    {
        process_label(logFile);   // Проверяем и обрабатываем метку в начале строки
        statement(logFile);       // Обрабатываем остальные лексемы
        match(Semicolon, logFile);  // Проверяем, что строка завершается точкой с запятой
    } while (TokenTable[pos].type != StopProgram);

}
// <оператор> ::= <присвоєння> | <ввід> | <вивід> | <умовний оператор> | <складений оператор>
void statement(FILE* logFile)
{

    switch (TokenTable[pos].type)
    {
    case Input: input(logFile); break;
    case Output: output(logFile); break;
    case If: conditional(logFile); break;
    case For: For_loop(logFile); break;
    case Goto: GoTo(logFile); break;
    case StartProgram: compound_statement(logFile); break;
    default: assignment(logFile);
    }
}
// <присвоєння> ::= <ідентифікатор> ':=' <арифметичний вираз>
void assignment(FILE* logFile)
{
    match(Identifier, logFile);
    match(Assign, logFile);
    if (TokenTable[pos].type == Sub)
    {
        match(Sub, logFile);
        arithmetic_expression(logFile);
    }
    else
    {
    arithmetic_expression(logFile);
    }
}
// <арифметичний вираз> ::= <доданок> { ('+' | '-') <доданок> }
void arithmetic_expression(FILE* logFile)
{
    term(logFile);
    while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub)
    {
        pos++;
        term(logFile);
    }
}
// <доданок> ::= <множник> { ('*' | '/') <доданок> }
void term(FILE* logFile)
{
    factor(logFile);
    while (TokenTable[pos].type == Mul || TokenTable[pos].type == Div || TokenTable[pos].type == Mod)
    {
        pos++;
        term(logFile);
    }
}
// <множник> ::= <ідентифікатор> | <число> | '(' <арифметичний вираз> ')'
void factor(FILE* logFile)
{
    if (TokenTable[pos].type == Identifier)
    {
        match(Identifier, logFile);
    }
    else if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);
    }
    else if (TokenTable[pos].type == LBraket)
    {
        match(LBraket, logFile);
        arithmetic_expression(logFile);
        match(RBraket, logFile);
    }
    else
    {
        fprintf(logFile, "\nSyntax error in line %d : A multiplier was expected.\n", TokenTable[pos].line);
        printf("\nSyntax error in line %d : A multiplier was expected.\n", TokenTable[pos].line);
        exit(1);
    }
}
// <вивід> ::= 'output' <ідентифікатор>
void output(FILE* logFile)
{
    match(Output, logFile);  // Проверяем, что токен — это 'Get'

    if (TokenTable[pos].type == Identifier) // Вывод идентификатора
    {
        match(Identifier, logFile);
    }
    else if (TokenTable[pos].type == Number) // Вывод числа
    {
        match(Number, logFile);
    }
    else if (TokenTable[pos].type == LBraket) // Вывод арифметического выражения
    {
        arithmetic_expression(logFile);
    }
    else
    {
        fprintf(logFile, "\nSyntax error in line %d: Expected identifier, number or arithmetic expression.\n", TokenTable[pos].line);
        printf("\nSyntax error in line %d: Expected identifier, number or arithmetic expression.\n", TokenTable[pos].line);
        exit(1);
    }
}
// <ввід> ::= 'input' <ідентифікатор>
void input(FILE* logFile)
{
    match(Input, logFile);
    match(Identifier, logFile);
}
// умовний_оператор> ::= 'If' <умова> 'ThenGoto' <мітка_1> 'Goto' <мітка_2>
void conditional(FILE* logFile)
{
    match(If, logFile);
    comparison(logFile);
    ThenGoTo(logFile);
}
// <операція порівняння> = <порівняння> | ‘!‘ ‘(‘ <логічний вираз> ‘)‘ | ‘(‘ <логічний вираз> ‘)‘
// <порівняння> = <арифметичний вираз> <менше-більше> <арифметичний вираз>
// <менше-більше> = ‘>‘ | ‘<‘ | ‘=‘ | ‘<>‘
void comparison(FILE* logFile)
{
    if (TokenTable[pos].type == Not)
    {
        // Варіант: ! (<логічний вираз>)
        pos++;
        match(LBraket, logFile);
        logical_expression(logFile);
        match(RBraket, logFile);
    }
    else if (TokenTable[pos].type == LBraket)
    {
        // Варіант: ( <логічний вираз> )
        pos++;
        logical_expression(logFile);
        match(RBraket, logFile);
    }
    else
    {
        // Варіант: <арифметичний вираз> <менше-більше> <арифметичний вираз>
        arithmetic_expression(logFile);
        if (TokenTable[pos].type == NotGreater || TokenTable[pos].type == NotLess ||
            TokenTable[pos].type == Equality || TokenTable[pos].type == NotEquality)
        {
            pos++;
            arithmetic_expression(logFile);
        }
        else
        {
            fprintf(logFile, "\nSyntax error: A comparison operation is expected.\n");
            printf("\nSyntax error: A comparison operation is expected.\n");
            exit(1);
        }
    }
}
// <логічний вираз> ::= <вираз І> { '|' <вираз І> }
void logical_expression(FILE* logFile)
{
    and_expression(logFile);
    while (TokenTable[pos].type == Or)
    {
        pos++;
        and_expression(logFile);
    }
}
// <вираз І> ::= <порівняння> { '&' <вираз І> }
void and_expression(FILE* logFile)
{
    comparison(logFile);
    while (TokenTable[pos].type == And)
    {
        pos++;
        and_expression(logFile);
    }
}
void ThenGoTo(FILE* logFile)
{
    match(Then, logFile);
    fprintf(logFile, "Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    match(Identifier, logFile);
    fprintf(logFile, "Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);

    if (matrixSize < 100)
    {
        strcpy_s(matrix[matrixSize++], TokenTable[pos - 1].name);
    }
    else
    {
        fprintf(logFile, "\nToo many labels defined!\n");
        exit(1);
    }
    match(Semicolon, logFile);
    fprintf(logFile, "Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    match(StartProgram, logFile);
    match(Semicolon, logFile);
    while (TokenTable[pos].type != StopProgram)
    {
        fprintf(logFile, "Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        program_body(logFile);
    }
    fprintf(logFile, "Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    match(StopProgram, logFile);
}
void GoTo(FILE* logFile)
{
    match(Goto, logFile);
    match(Identifier, logFile);

    if (matrixSize < 100)
    {
        strcpy_s(matrix[matrixSize++], TokenTable[pos - 1].name);
    }
    else
    {
        fprintf(logFile, "\nToo many labels defined!\n");
        exit(1);
    }
}
void process_label(FILE*logFile)
{
    // Метка может быть только в начале строки, и только если массив matrix не пуст
    if (matrixSize > 0 && TokenTable[pos].type == Identifier)
    {
        // Сохраняем метку
        if (matrixSize < 100)
        {
            strcpy_s(matrix[matrixSize++], TokenTable[pos].name);
        }
        else
        {
            fprintf(logFile, "\nToo many labels defined!\n");
            exit(1);
        }
        pos++; // Переходим к следующему токену
    }
}
void For_loop(FILE* logFile)
{
    match(For, logFile);                          // Ожидаем ключевое слово 'For'
    match(Identifier, logFile);                   // Ожидаем идентификатор (счётчик)
    match(Equality, logFile);                   // Ожидаем символы '='
    initial_value(logFile);               // Обрабатываем <початкове_значення>
    match(To, logFile);                           // Ожидаем ключевое слово 'To'
    if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);                   // Ожидаем число
    }
    else
    {
        arithmetic_expression(logFile);   // Либо обрабатываем арифметическое выражение
    }
    // Проверяем наличие 'STEP' и обрабатываем шаг, если он есть
    if (TokenTable[pos].type == Step)
    {
        match(Step, logFile);
        step(logFile);                    // Обрабатываем <крок>
    }
    match(Semicolon, logFile);
    loop_body(logFile);                   // Обрабатываем <тіло_циклу>
    match(Next, logFile);                         // Ожидаем ключевое слово 'Next'
    match(Identifier, logFile);                   // Проверяем, что идентификатор совпадает с начальным
}
// Функция для обработки начального значения <початкове_значення>
void initial_value(FILE* logFile)
{
    if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);                   // Начальное значение — число
    }
    else
    {
        arithmetic_expression(logFile);   // Или арифметическое выражение
    }
}
// Функция для обработки шага <крок>
void step(FILE* logFile)
{
    if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);                   // Шаг — число
    }
    else
    {
        arithmetic_expression(logFile);   // Или арифметическое выражение
    }
}
// Функция для обработки тела цикла <тіло_циклу>
void loop_body(FILE* logFile)
{
    while (TokenTable[pos].type != Next)
    {
        statement(logFile);                     // Обрабатываем операторы в теле цикла
        match(Semicolon, logFile);
    }
}
// <складений оператор> ::= 'start' <тіло програми> 'stop'
void compound_statement(FILE* logFile)
{
    match(StartProgram, logFile);
    program_body(logFile);
    match(StopProgram, logFile);
}
// функція записує оголошені ідентифікатори в таблицю ідентифікаторів IdTable
// повертає кількість ідентифікаторів
// перевіряє чи усі використані ідентифікатори оголошені
unsigned int IdIdentification(Id IdTable[], Token TokenTable[], unsigned int tokenCount, FILE* logFile)
{
    unsigned int idCount = 0;
    unsigned int i = 0;

    while (TokenTable[i++].type != Variable);

    if (TokenTable[i++].type == Type)
    {
        while (TokenTable[i].type != Semicolon)
        {
            if (TokenTable[i].type == Identifier)
            {
                int yes = 0;
                for (unsigned int j = 0; j < idCount; j++)
                {
                    if (!strcmp(TokenTable[i].name, IdTable[j].name))
                    {
                        yes = 1;
                        break;
                    }
                }
                if (yes == 1)
                {
                    printf("\nidentifier \"%s\" is already declared !\n", TokenTable[i].name);
                    fprintf(logFile, "\nidentifier \"%s\" is already declared !\n", TokenTable[i].name);
                    return idCount;
                }
                if (idCount < MAX_IDENTIFIER)
                {
                    strcpy_s(IdTable[idCount++].name, TokenTable[i++].name);
                }
                else
                {
                    printf("\nToo many identifiers !\n");
                    fprintf(logFile, "\nToo many identifiers !\n");
                    return idCount;
                }
            }
            else { i++; }
        }
    }
    for (; i < tokenCount; i++)
    {
        if (TokenTable[i].type == Identifier)
        {
            int yes = 0;
            for (unsigned int j = 0; j < idCount; j++)
            {
                if (!strcmp(TokenTable[i].name, IdTable[j].name))
                {
                    yes = 1;
                    break;
                }
            }
            // Проверяем, является ли идентификатор меткой (находится ли он в матрице matrix)
            int isLabel = 0;
            for (unsigned int k = 0; k < matrixSize; k++)
            {
                if (!strcmp(TokenTable[i].name, matrix[k]))
                {
                    isLabel = 1;
                    break;
                }
            }
            if (isLabel == 1)
            {
                printf("\nIn line %d,  a label\"%s\"!", TokenTable[i].line, TokenTable[i].name);
                fprintf(logFile, "\nIn line %d, found a label\"%s\"!", TokenTable[i].line, TokenTable[i].name);
            }
            else if (yes == 0 && isLabel == 0)
            {
                printf("\nIn line %d, an undeclared identifier \"%s\"!", TokenTable[i].line, TokenTable[i].name);
                fprintf(logFile, "\nIn line %d, an undeclared identifier \"%s\"!", TokenTable[i].line, TokenTable[i].name);
            }
        }
    }
    return idCount; // Повертає кількість  ідентифікаторів
}
