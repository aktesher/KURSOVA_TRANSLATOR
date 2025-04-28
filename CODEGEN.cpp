
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Header.h"
// таблиця лексем
extern Token* TokenTable;
// кількість лексем
extern unsigned int TokensNum;
// таблиця ідентифікаторів
extern Id* IdTable;
// кількість ідентифікаторів
extern unsigned int IdNum;
static int pos = 0;
char Matrix[100][8]; // Хранит до 100 меток, каждая длиной до 7 символов + \0
unsigned int MatrixSize = 0;

// набір функцій для рекурсивного спуску 
// на кожне правило - окрема функція
void Variable_declaration(FILE* outFile);
void Variable_list(FILE* outFile);
void Program_body(FILE* outFile);
void Statement(FILE* outFile);
void Assignment(FILE* outFile);
void Arithmetic_expression(FILE* outFile);
void Term(FILE* outFile);
void Factor(FILE* outFile);
void inpuT(FILE* outFile);
void outpuT(FILE* outFile);
void Conditional(FILE* outFile);
void Logical_expression(FILE* outFile);
void And_expression(FILE* outFile);
void Comparison(FILE* outFile);
void Compound_statement(FILE* outFile);
void ThenGo_To(FILE* outFile);
void Go_To(FILE* outFile);
void Initial_value(FILE* outFile);
void Loop_body(FILE* outFile);
void steP(FILE* outFile);
void For_Loop(FILE* outFile);
void process_Label(FILE* outFile);

void generateCCode(FILE* outFile)
{
    if (TokenTable[pos].type == ProgramName)
    {
        pos++;
        if (TokenTable[pos].type == Identifier)
        {
            pos++;
            if (TokenTable[pos].type == Semicolon)
            {
                pos++;
                fprintf(outFile, "#include <iostream>\n");
                fprintf(outFile, "using namespace std;\n\n");
            }
            else { printf("\nSyntax error: incorrect end of line.\n"); exit(1); }
        }
        else{printf("\nSyntax error: undeclared program name.\n"); exit(1);}
    }
    else{printf("\nSyntax error: undeclared program start.\n"); exit(1);}

    if (TokenTable[pos].type == StartProgram)
    {
        //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        pos++;
        fprintf(outFile, "int main() \n{\n");
      //  Compound_statement(outFile);
    }
    Variable_declaration(outFile);
    fprintf(outFile, ";\n");
    pos++; 
    //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    Program_body(outFile);
    fprintf(outFile, "   return 0;\n");
    fprintf(outFile, "}\n");
}
// <оголошення змінних> = [<тип даних> <список змінних>]
void Variable_declaration(FILE* outFile)
{
    if (TokenTable[pos].type == Variable)
    {
        pos++;
        if (TokenTable[pos].type == Type)
        {
            fprintf(outFile, "   int ");
            pos++;
            Variable_list(outFile);
        }
    }
    else 
    {
        printf("\nSyntax error: variables daclaration not found.\n");
    }
}
// <список змінних> = <ідентифікатор> { ',' <ідентифікатор> }
void Variable_list(FILE* outFile)
{
    fprintf(outFile, TokenTable[pos++].name);
    while (TokenTable[pos].type == Comma)
    {
        fprintf(outFile, ", ");
        pos++;
        fprintf(outFile, TokenTable[pos++].name);
    }
}
// <тіло програми> = <оператор> ';' { <оператор> ';' }
void Program_body(FILE* outFile)
{
    do
    {
        process_Label(outFile);
        Statement(outFile);
        pos++;
    }
    while (TokenTable[pos].type != StopProgram);
}
// <оператор> = <присвоєння> | <ввід> | <вивід> | <умовний оператор> | <складений оператор>
void Statement(FILE* outFile)
{
    //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    switch (TokenTable[pos].type)
    {
    case Input: inpuT(outFile); break;
    case Output: outpuT(outFile); break;
    case If: Conditional(outFile); break;
    case For: For_Loop(outFile); break;
    case Goto: Go_To(outFile); break;
    case StartProgram: Compound_statement(outFile); break;
    default: Assignment(outFile);
    }
}
// <присвоєння> = <ідентифікатор> ':=' <арифметичний вираз>
void Assignment(FILE* outFile)
{
    //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    pos++;
    fprintf(outFile, " = ");
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    pos++;
    //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    if (TokenTable[pos].type == Sub) 
    {
        fprintf(outFile, "-");
        pos++;
        //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        Arithmetic_expression(outFile);
        fprintf(outFile, ";\n");
    }
    else 
    {
        Arithmetic_expression(outFile);
        fprintf(outFile, ";\n");
    }
    //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
}
// <арифметичний вираз> = <доданок> { ('+' | '-') <доданок> }
void Arithmetic_expression(FILE* outFile)
{
    //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    Term(outFile);
    while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub)
    {
       // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        if (TokenTable[pos].type == Add)
            fprintf(outFile, " + ");
        else if (TokenTable[pos].type == Sub)
            fprintf(outFile, " - ");
        pos++;
       // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        Term(outFile);
    }
}
// <доданок> = <множник> { ('*' | '/') <множник> }
void Term(FILE* outFile)
{
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    Factor(outFile);
    while (TokenTable[pos].type == Mul || TokenTable[pos].type == Div || TokenTable[pos].type == Mod)
    {
        if (TokenTable[pos].type == Mul)
            fprintf(outFile, " * ");
        else if (TokenTable[pos].type == Div)
            fprintf(outFile, " / ");
        else if (TokenTable[pos].type == Mod)
            fprintf(outFile, " %% ");
        pos++;
        Factor(outFile);
    }
}
// <множник> = <ідентифікатор> | <число> | '(' <арифметичний вираз> ')'
void Factor(FILE* outFile)
{
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    if (TokenTable[pos].type == LBraket)
    {
        fprintf(outFile, "(");
        pos++;
        Arithmetic_expression(outFile);
        fprintf(outFile, ")");
        pos++;
    }
    else  if (TokenTable[pos].type == Identifier || TokenTable[pos].type == Number)
    {
        fprintf(outFile, TokenTable[pos++].name);
    }
}
// <вивід> = 'Output' <ідентифікатор>
void outpuT(FILE* outFile)
{
    fprintf(outFile, "   cout << \"\\t\" << ");
    pos++;
    Arithmetic_expression(outFile);
    fprintf(outFile, " << \" \\n\";\n");
}
// <ввід> = 'Input' <ідентифікатор>
void inpuT(FILE* outFile)
{
    fprintf(outFile, "   cout << \"\\t\"<< \"ENTER ");  
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    fprintf(outFile, TokenTable[pos + 1].name);
    fprintf(outFile, ":\"; \n");
    fprintf(outFile, "   cin >> ");
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    pos++;
    fprintf(outFile, "%s", TokenTable[pos++].name);
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    fprintf(outFile, ";\n");
}
// умовний_оператор> ::= 'If' <умова> 'ThenGo_To' <мітка_1> 'Go_To' <мітка_2>
void Conditional(FILE* outFile)
{
    fprintf(outFile, "   if (");
    pos++;  
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    Comparison(outFile);
    fprintf(outFile, ")\n");
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    ThenGo_To(outFile);
}
// <складений оператор> = 'start' <тіло програми> 'stop'
void ThenGo_To(FILE* outFile)
{
    if (TokenTable[pos].type == Then)
    {
        pos++;
     //   printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        // Печатаем тело цикла
        if (TokenTable[pos].type == Identifier)
        {
            pos++;
            if (TokenTable[pos].type == Semicolon)
            {
                pos++;
                if (TokenTable[pos].type == StartProgram)
                {
                    pos++;
                    if (TokenTable[pos].type == Semicolon)
                    {// Открываем блок фигурных скобок
                        fprintf(outFile, "   {\n"); 
                        pos++;
                        if (TokenTable[pos].type == Identifier)
                        {
                            pos++;
                            while (TokenTable[pos].type != StopProgram)
                            {
                              //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
                                if (TokenTable[pos].type != If && TokenTable[pos].type != Output &&
                                    TokenTable[pos].type != Input && TokenTable[pos].type != Goto)
                                {
                                    fprintf(outFile, "   %s", TokenTable[pos].name);
                                }
                                Statement(outFile);
                             //   printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
                                pos++;
                            }
                         //   printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
                            if (TokenTable[pos].type == StopProgram)
                            {
                           //     printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
                                while (TokenTable[pos].type != Semicolon)
                                {
                                    pos++;
                               //     printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
                                }
                            }
                        }
                        else { printf("\nSyntax error: second conditional label not found.\n"); exit(1); }
                    }
                    else { printf("\nSyntax error: incorrect end of line.\n"); exit(1); }
                }
                else { printf("\nSyntax error: compound statement not found.\n"); exit(1); }
            }
            else { printf("\nSyntax error: incorrect end of line.\n"); exit(1); }
        }
        else { printf("\nSyntax error: first conditional label not found.\n"); exit(1); }
    }
    fprintf(outFile, "   }\n"); // Закрываем блок фигурных скобок
}
// <порівняння> = <операція порівняння> | ‘!‘ ‘(‘ <логічний вираз> ‘)‘ | ‘(‘ <логічний вираз> ‘)‘
// <операція порівняння> = <арифметичний вираз> <менше-більше> <арифметичний вираз>
// <менше-більше> = ‘>‘ | ‘<‘ | ‘=‘ | ‘<>‘
void Comparison(FILE* outFile)
{
    if (TokenTable[pos].type == Not)
    {// Варіант: ! (<логічний вираз>)
        fprintf(outFile, "!(");
        pos++;
        pos++;
        Logical_expression(outFile);
        fprintf(outFile, ")");
        pos++;
    }
    else if (TokenTable[pos].type == LBraket)
    {// Варіант: ( <логічний вираз> )
        fprintf(outFile, "(");
        pos++;
        Logical_expression(outFile);
        fprintf(outFile, ")");
        pos++;
    }
    else
    {// Варіант: <арифметичний вираз> <менше-більше> <арифметичний вираз>
        Arithmetic_expression(outFile);
        if (TokenTable[pos].type == NotGreater || TokenTable[pos].type == NotLess ||
            TokenTable[pos].type == Equality || TokenTable[pos].type == NotEquality)
        {
            switch (TokenTable[pos].type)
            {
            case NotGreater: fprintf(outFile, " <= "); break;
            case NotLess: fprintf(outFile, " >= "); break;
            case Equality: fprintf(outFile, " == "); break;
            case NotEquality: fprintf(outFile, " != "); break;
            }
            pos++;
            Arithmetic_expression(outFile);
        }
    }
}
// <логічний вираз> = <вираз І> { '|' <вираз І> }
void Logical_expression(FILE* outFile)
{
    And_expression(outFile);
    while (TokenTable[pos].type == Or)
    {
        fprintf(outFile, " || ");
        pos++;
        And_expression(outFile);
    }
}
// <вираз І> = <порівняння> { '&' <порівняння> }
void And_expression(FILE* outFile)
{
    Comparison(outFile);
    while (TokenTable[pos].type == And)
    {
        fprintf(outFile, " && ");
        pos++;
        Comparison(outFile);
    }
}
void Go_To(FILE* outFile)
{
    fprintf(outFile, "   goto ");
    pos++;
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    // Проверяем, существует ли идентификатор после Goto
    if (TokenTable[pos].type == Identifier)
    {
        // Записываем идентификатор в C++ код
        fprintf(outFile, "%s;\n", TokenTable[pos].name);
        // Проверяем, если идентификатор уже существует в массиве Matrix
        int isLabelAlreadyAdded = 0;
        for (unsigned int i = 0; i < MatrixSize; i++)
        {
            if (strcmp(Matrix[i], TokenTable[pos].name) == 0)
            {
                isLabelAlreadyAdded = 1;
                break;
            }
        }
        // Если метка ещё не записана, добавляем её в Matrix
        if (!isLabelAlreadyAdded)
        {
            if (MatrixSize < 100)
            {
                strcpy_s(Matrix[MatrixSize++], TokenTable[pos].name);
            }
            else
            {
                printf("\nToo many labels defined!\n");
                exit(1);
            }
        }
        pos++; // Переходим к следующему токену
    }
    else
    {
        printf("\nSyntax error: Expected identifier after Goto.\n");
        exit(1);
    }
    printf("Matrix of labels contents:\n");
    for (unsigned int i = 0; i < MatrixSize; i++) 
    {
        printf("%s\n", Matrix[i]); // Печатаем каждую строку массива
    }
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
}
void process_Label(FILE* outFile)
{
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    // Проверяем, есть ли идентификатор в начале строки
    if (TokenTable[pos].type == Identifier)
    {
        // Проверяем, есть ли идентификатор в массиве Matrix
        int isLabel = 0;
        for (unsigned int i = 0; i < MatrixSize; i++)
        {
            if (strcmp(Matrix[i], TokenTable[pos].name) == 0)
            {
                isLabel = 1;
                break;
            }
        }
        // Если это метка, записываем её в C++ код как label:
        if (isLabel)
        {
            fprintf(outFile, "%s:  \n", TokenTable[pos].name);
            pos++;
        }
        else
        {
            // Если идентификатор не найден в матрице, записываем его как обычный идентификатор
            fprintf(outFile, "   %s", TokenTable[pos].name);
        }
      //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    }
}
void For_Loop(FILE* outFile)
{
    fprintf(outFile, "   for (");
    Initial_value(outFile); // Начальное значение переменной цикла
    fprintf(outFile, "; ");
    pos++; 
    // Условие окончания цикла
  // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    fprintf(outFile, "%s", TokenTable[pos - 4].name); // Переменная цикла ????
    fprintf(outFile, " < ");
    Arithmetic_expression(outFile); // Верхняя граница цикла
    fprintf(outFile, "; ");
    steP(outFile); // Шаг итерации
    fprintf(outFile, ")\n");
    Loop_body(outFile); // Тело цикла  
}
void Loop_body(FILE* outFile)
{
    fprintf(outFile, "   {\n");
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    pos++; // Переходим к первому оператору внутри цикла
    while (TokenTable[pos].type != Next)
    {
      //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        process_Label(outFile);
        Statement(outFile); // Обработка операторов в теле цикла
        pos++;
       // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    }
    fprintf(outFile, "   }\n");
    pos++; // Пропускаем Next
    pos++; // Пропускаем Next
 //   printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
}
void Initial_value(FILE* outFile)
{
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    pos++;
 //   printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    if (TokenTable[pos].type == Identifier)
    {
        fprintf(outFile, "int %s = ", TokenTable[pos].name); // Имя переменной цикла
        pos++;
        if (TokenTable[pos].type == Equality)
        {
            pos++;
        //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
            Arithmetic_expression(outFile);
        }
    }
}
void steP(FILE* outFile)
{
    if (TokenTable[pos].type == Step)
    {
        pos++; // Пропускаем Step
       // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        fprintf(outFile, "%s += ", TokenTable[pos - 6].name); // Имя переменной цикла
        Arithmetic_expression(outFile); // Шаг итерации
    }
    else
    {
        fprintf(outFile, "%s++", TokenTable[pos - 5].name); // Шаг по умолчанию
    }
}
void Compound_statement(FILE* outFile)
{
    fprintf(outFile, "   {\n");
    pos++;
    Program_body(outFile);
    fprintf(outFile, "   }\n");
    pos++;
}
