#include <iostream>
#include "Header.h"

// таблиц€ лексем
extern Token* TokenTable;
// к≥льк≥сть лексем
extern unsigned int TokensNum;
// таблиц€ ≥дентиф≥катор≥в
extern Id* IdTable;
extern // к≥льк≥сть ≥дентиф≥катор≥в
unsigned int IdNum;

int pos = 0;
char matrix[100][8]; // —охран€ем до 100 меток, кажда€ до 7 символов + \0
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
// <програма> ::= '#Program'<≥дентиф≥катор>';' 'Start' 'Variable' <оголошенн€_зм≥нних> ';' <т≥ло_програми>  'Stop' 
void program(FILE* logFile)
{
    match(ProgramName, logFile);    // Ќовый токен дл€ начала программы
    match(Identifier, logFile);          // »м€ программы
    match(Semicolon, logFile);         // “очка с зап€той
    match(StartProgram, logFile);     // Ќачало основной части программы
    match(Variable, logFile);
    variable_declaration(logFile);
    match(Semicolon, logFile);
    program_body(logFile);
    match(StopProgram, logFile);
}
// <оголошенн€ зм≥нних> ::= [<тип даних> <список зм≥нних>]
void variable_declaration(FILE* logFile)
{
    if (TokenTable[pos].type == Type)
    {
        pos++;
        variable_list(logFile);
    }
}
// <список зм≥нних> ::= <≥дентиф≥катор> { ',' <≥дентиф≥катор> }
void variable_list(FILE* logFile)
{
    match(Identifier, logFile);
    while (TokenTable[pos].type == Comma)
    {
        pos++;
        match(Identifier, logFile);
    }
}
// <т≥ло програми> ::= <оператор> ';' { <оператор> ';' }
void program_body(FILE* logFile)
{
    do
    {
        process_label(logFile);   // ѕровер€ем и обрабатываем метку в начале строки
        statement(logFile);       // ќбрабатываем остальные лексемы
        match(Semicolon, logFile);  // ѕровер€ем, что строка завершаетс€ точкой с зап€той
    } while (TokenTable[pos].type != StopProgram);

}
// <оператор> ::= <присвоЇнн€> | <вв≥д> | <вив≥д> | <умовний оператор> | <складений оператор>
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
// <присвоЇнн€> ::= <≥дентиф≥катор> ':=' <арифметичний вираз>
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
// <множник> ::= <≥дентиф≥катор> | <число> | '(' <арифметичний вираз> ')'
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
// <вив≥д> ::= 'output' <≥дентиф≥катор>
void output(FILE* logFile)
{
    match(Output, logFile);  // ѕровер€ем, что токен Ч это 'Get'

    if (TokenTable[pos].type == Identifier) // ¬ывод идентификатора
    {
        match(Identifier, logFile);
    }
    else if (TokenTable[pos].type == Number) // ¬ывод числа
    {
        match(Number, logFile);
    }
    else if (TokenTable[pos].type == LBraket) // ¬ывод арифметического выражени€
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
// <вв≥д> ::= 'input' <≥дентиф≥катор>
void input(FILE* logFile)
{
    match(Input, logFile);
    match(Identifier, logFile);
}
// умовний_оператор> ::= 'If' <умова> 'ThenGoto' <м≥тка_1> 'Goto' <м≥тка_2>
void conditional(FILE* logFile)
{
    match(If, logFile);
    comparison(logFile);
    ThenGoTo(logFile);
}
// <операц≥€ пор≥вн€нн€> = <пор≥вн€нн€> | С!С С(С <лог≥чний вираз> С)С | С(С <лог≥чний вираз> С)С
// <пор≥вн€нн€> = <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
// <менше-б≥льше> = С>С | С<С | С=С | С<>С
void comparison(FILE* logFile)
{
    if (TokenTable[pos].type == Not)
    {
        // ¬ар≥ант: ! (<лог≥чний вираз>)
        pos++;
        match(LBraket, logFile);
        logical_expression(logFile);
        match(RBraket, logFile);
    }
    else if (TokenTable[pos].type == LBraket)
    {
        // ¬ар≥ант: ( <лог≥чний вираз> )
        pos++;
        logical_expression(logFile);
        match(RBraket, logFile);
    }
    else
    {
        // ¬ар≥ант: <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
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
// <лог≥чний вираз> ::= <вираз ≤> { '|' <вираз ≤> }
void logical_expression(FILE* logFile)
{
    and_expression(logFile);
    while (TokenTable[pos].type == Or)
    {
        pos++;
        and_expression(logFile);
    }
}
// <вираз ≤> ::= <пор≥вн€нн€> { '&' <вираз ≤> }
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
    // ћетка может быть только в начале строки, и только если массив matrix не пуст
    if (matrixSize > 0 && TokenTable[pos].type == Identifier)
    {
        // —охран€ем метку
        if (matrixSize < 100)
        {
            strcpy_s(matrix[matrixSize++], TokenTable[pos].name);
        }
        else
        {
            fprintf(logFile, "\nToo many labels defined!\n");
            exit(1);
        }
        pos++; // ѕереходим к следующему токену
    }
}
void For_loop(FILE* logFile)
{
    match(For, logFile);                          // ќжидаем ключевое слово 'For'
    match(Identifier, logFile);                   // ќжидаем идентификатор (счЄтчик)
    match(Equality, logFile);                   // ќжидаем символы '='
    initial_value(logFile);               // ќбрабатываем <початкове_значенн€>
    match(To, logFile);                           // ќжидаем ключевое слово 'To'
    if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);                   // ќжидаем число
    }
    else
    {
        arithmetic_expression(logFile);   // Ћибо обрабатываем арифметическое выражение
    }
    // ѕровер€ем наличие 'STEP' и обрабатываем шаг, если он есть
    if (TokenTable[pos].type == Step)
    {
        match(Step, logFile);
        step(logFile);                    // ќбрабатываем <крок>
    }
    match(Semicolon, logFile);
    loop_body(logFile);                   // ќбрабатываем <т≥ло_циклу>
    match(Next, logFile);                         // ќжидаем ключевое слово 'Next'
    match(Identifier, logFile);                   // ѕровер€ем, что идентификатор совпадает с начальным
}
// ‘ункци€ дл€ обработки начального значени€ <початкове_значенн€>
void initial_value(FILE* logFile)
{
    if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);                   // Ќачальное значение Ч число
    }
    else
    {
        arithmetic_expression(logFile);   // »ли арифметическое выражение
    }
}
// ‘ункци€ дл€ обработки шага <крок>
void step(FILE* logFile)
{
    if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);                   // Ўаг Ч число
    }
    else
    {
        arithmetic_expression(logFile);   // »ли арифметическое выражение
    }
}
// ‘ункци€ дл€ обработки тела цикла <т≥ло_циклу>
void loop_body(FILE* logFile)
{
    while (TokenTable[pos].type != Next)
    {
        statement(logFile);                     // ќбрабатываем операторы в теле цикла
        match(Semicolon, logFile);
    }
}
// <складений оператор> ::= 'start' <т≥ло програми> 'stop'
void compound_statement(FILE* logFile)
{
    match(StartProgram, logFile);
    program_body(logFile);
    match(StopProgram, logFile);
}
// функц≥€ записуЇ оголошен≥ ≥дентиф≥катори в таблицю ≥дентиф≥катор≥в IdTable
// повертаЇ к≥льк≥сть ≥дентиф≥катор≥в
// перев≥р€Ї чи ус≥ використан≥ ≥дентиф≥катори оголошен≥
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
            // ѕровер€ем, €вл€етс€ ли идентификатор меткой (находитс€ ли он в матрице matrix)
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
    return idCount; // ѕовертаЇ к≥льк≥сть  ≥дентиф≥катор≥в
}
