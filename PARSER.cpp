#include <iostream>
#include "Header.h"

// ������� ������
extern Token* TokenTable;
// ������� ������
extern unsigned int TokensNum;
// ������� ��������������
extern Id* IdTable;
extern // ������� ��������������
unsigned int IdNum;

int pos = 0;
char matrix[100][8]; // ��������� �� 100 �����, ������ �� 7 �������� + \0
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
// <��������> ::= '#Program'<�������������>';' 'Start' 'Variable' <����������_������> ';' <���_��������>  'Stop' 
void program(FILE* logFile)
{
    match(ProgramName, logFile);    // ����� ����� ��� ������ ���������
    match(Identifier, logFile);          // ��� ���������
    match(Semicolon, logFile);         // ����� � �������
    match(StartProgram, logFile);     // ������ �������� ����� ���������
    match(Variable, logFile);
    variable_declaration(logFile);
    match(Semicolon, logFile);
    program_body(logFile);
    match(StopProgram, logFile);
}
// <���������� ������> ::= [<��� �����> <������ ������>]
void variable_declaration(FILE* logFile)
{
    if (TokenTable[pos].type == Type)
    {
        pos++;
        variable_list(logFile);
    }
}
// <������ ������> ::= <�������������> { ',' <�������������> }
void variable_list(FILE* logFile)
{
    match(Identifier, logFile);
    while (TokenTable[pos].type == Comma)
    {
        pos++;
        match(Identifier, logFile);
    }
}
// <��� ��������> ::= <��������> ';' { <��������> ';' }
void program_body(FILE* logFile)
{
    do
    {
        process_label(logFile);   // ��������� � ������������ ����� � ������ ������
        statement(logFile);       // ������������ ��������� �������
        match(Semicolon, logFile);  // ���������, ��� ������ ����������� ������ � �������
    } while (TokenTable[pos].type != StopProgram);

}
// <��������> ::= <���������> | <���> | <����> | <������� ��������> | <��������� ��������>
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
// <���������> ::= <�������������> ':=' <������������ �����>
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
// <������������ �����> ::= <�������> { ('+' | '-') <�������> }
void arithmetic_expression(FILE* logFile)
{
    term(logFile);
    while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub)
    {
        pos++;
        term(logFile);
    }
}
// <�������> ::= <�������> { ('*' | '/') <�������> }
void term(FILE* logFile)
{
    factor(logFile);
    while (TokenTable[pos].type == Mul || TokenTable[pos].type == Div || TokenTable[pos].type == Mod)
    {
        pos++;
        term(logFile);
    }
}
// <�������> ::= <�������������> | <�����> | '(' <������������ �����> ')'
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
// <����> ::= 'output' <�������������>
void output(FILE* logFile)
{
    match(Output, logFile);  // ���������, ��� ����� � ��� 'Get'

    if (TokenTable[pos].type == Identifier) // ����� ��������������
    {
        match(Identifier, logFile);
    }
    else if (TokenTable[pos].type == Number) // ����� �����
    {
        match(Number, logFile);
    }
    else if (TokenTable[pos].type == LBraket) // ����� ��������������� ���������
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
// <���> ::= 'input' <�������������>
void input(FILE* logFile)
{
    match(Input, logFile);
    match(Identifier, logFile);
}
// �������_��������> ::= 'If' <�����> 'ThenGoto' <����_1> 'Goto' <����_2>
void conditional(FILE* logFile)
{
    match(If, logFile);
    comparison(logFile);
    ThenGoTo(logFile);
}
// <�������� ���������> = <���������> | �!� �(� <������� �����> �)� | �(� <������� �����> �)�
// <���������> = <������������ �����> <�����-�����> <������������ �����>
// <�����-�����> = �>� | �<� | �=� | �<>�
void comparison(FILE* logFile)
{
    if (TokenTable[pos].type == Not)
    {
        // ������: ! (<������� �����>)
        pos++;
        match(LBraket, logFile);
        logical_expression(logFile);
        match(RBraket, logFile);
    }
    else if (TokenTable[pos].type == LBraket)
    {
        // ������: ( <������� �����> )
        pos++;
        logical_expression(logFile);
        match(RBraket, logFile);
    }
    else
    {
        // ������: <������������ �����> <�����-�����> <������������ �����>
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
// <������� �����> ::= <����� �> { '|' <����� �> }
void logical_expression(FILE* logFile)
{
    and_expression(logFile);
    while (TokenTable[pos].type == Or)
    {
        pos++;
        and_expression(logFile);
    }
}
// <����� �> ::= <���������> { '&' <����� �> }
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
    // ����� ����� ���� ������ � ������ ������, � ������ ���� ������ matrix �� ����
    if (matrixSize > 0 && TokenTable[pos].type == Identifier)
    {
        // ��������� �����
        if (matrixSize < 100)
        {
            strcpy_s(matrix[matrixSize++], TokenTable[pos].name);
        }
        else
        {
            fprintf(logFile, "\nToo many labels defined!\n");
            exit(1);
        }
        pos++; // ��������� � ���������� ������
    }
}
void For_loop(FILE* logFile)
{
    match(For, logFile);                          // ������� �������� ����� 'For'
    match(Identifier, logFile);                   // ������� ������������� (�������)
    match(Equality, logFile);                   // ������� ������� '='
    initial_value(logFile);               // ������������ <���������_��������>
    match(To, logFile);                           // ������� �������� ����� 'To'
    if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);                   // ������� �����
    }
    else
    {
        arithmetic_expression(logFile);   // ���� ������������ �������������� ���������
    }
    // ��������� ������� 'STEP' � ������������ ���, ���� �� ����
    if (TokenTable[pos].type == Step)
    {
        match(Step, logFile);
        step(logFile);                    // ������������ <����>
    }
    match(Semicolon, logFile);
    loop_body(logFile);                   // ������������ <���_�����>
    match(Next, logFile);                         // ������� �������� ����� 'Next'
    match(Identifier, logFile);                   // ���������, ��� ������������� ��������� � ���������
}
// ������� ��� ��������� ���������� �������� <���������_��������>
void initial_value(FILE* logFile)
{
    if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);                   // ��������� �������� � �����
    }
    else
    {
        arithmetic_expression(logFile);   // ��� �������������� ���������
    }
}
// ������� ��� ��������� ���� <����>
void step(FILE* logFile)
{
    if (TokenTable[pos].type == Number)
    {
        match(Number, logFile);                   // ��� � �����
    }
    else
    {
        arithmetic_expression(logFile);   // ��� �������������� ���������
    }
}
// ������� ��� ��������� ���� ����� <���_�����>
void loop_body(FILE* logFile)
{
    while (TokenTable[pos].type != Next)
    {
        statement(logFile);                     // ������������ ��������� � ���� �����
        match(Semicolon, logFile);
    }
}
// <��������� ��������> ::= 'start' <��� ��������> 'stop'
void compound_statement(FILE* logFile)
{
    match(StartProgram, logFile);
    program_body(logFile);
    match(StopProgram, logFile);
}
// ������� ������ �������� �������������� � ������� �������������� IdTable
// ������� ������� ��������������
// �������� �� �� ���������� �������������� ��������
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
            // ���������, �������� �� ������������� ������ (��������� �� �� � ������� matrix)
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
    return idCount; // ������� �������  ��������������
}
