#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Header.h"

// таблиця лексем
extern Token* TokenTable;
// кількість лексем
extern unsigned int TokensNum;
static int pos = 0;
// функція створення вузла AST
ASTNode* createNode(TypeOfNodes type, const char* name, ASTNode* left, ASTNode* right)
{
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->nodetype = type;
    strcpy_s(node->name, name);
    node->left = left;
    node->right = right;
    return node;
}
// функція знищення дерева
void destroyTree(ASTNode* root)
{
    if (root == NULL)
        return;
    // Рекурсивно знищуємо ліве і праве піддерево
    destroyTree(root->left);
    destroyTree(root->right);
    // Звільняємо пам'ять для поточного вузла
    free(root);
}
// функція для друку AST у вигляді дерева на екран
void PrintAST(ASTNode* node, int level)
{
    if (node == NULL)
        return;
    // Відступи для позначення рівня вузла
    for (int i = 0; i < level; i++)
        printf("|   ");
    // Виводимо інформацію про вузол
    printf("|-- %s", node->name);
    printf("\n");
    // Рекурсивний друк лівого та правого піддерева
    if (node->left || node->right)
    {
        PrintAST(node->left, level + 1);
        PrintAST(node->right, level + 1);
    }
}
// функція для друку AST у вигляді дерева у файл
void PrintASTToFile(ASTNode* node, int level, FILE* outFile)
{
    if (node == NULL)
        return;
    // Відступи для позначення рівня вузла
    for (int i = 0; i < level; i++)
        fprintf(outFile, "|   ");
    // Виводимо інформацію про вузол
    fprintf(outFile, "|-- %s", node->name);
    fprintf(outFile, "\n");
    // Рекурсивний друк лівого та правого піддерева
    if (node->left || node->right)
    {
        PrintASTToFile(node->left, level + 1, outFile);
        PrintASTToFile(node->right, level + 1, outFile);
    }
}
// функція синтаксичного аналізу і створення абстрактного синтаксичного дерева
ASTNode* ParserAST()
{
    ASTNode* tree = Aprogram();
    printf("\nParsing completed. AST created.\n");
    return tree;
}
static void Amatch(TypeOfTokens expectedType)
{
    if (TokenTable[pos].type == expectedType)
        pos++;
    else
    {
        printf("\nSyntax error in line %d: Expected another type of lexeme.\n", TokenTable[pos].line);
        exit(10);
    }
}
// <програма> = 'start' 'var' <оголошення змінних> ';' <тіло програми> 'stop'
ASTNode* Aprogram()
{
    Amatch(ProgramName);
    Amatch(Identifier);
    Amatch(Semicolon);
    Amatch(StartProgram);
    Amatch(Variable);
    ASTNode* declarations = Avariable_declaration();
    Amatch(Semicolon);
    ASTNode* body = Aprogram_body();
    Amatch(StopProgram);
    return createNode(program_node, "#Program", declarations, body);
}
// <оголошення змінних> = [<тип даних> <список змінних>]
ASTNode* Avariable_declaration()
{
    if (TokenTable[pos].type == Type)
    {
        pos++;
        return Avariable_list();
    }
    return NULL;
}
// <список змінних> = <ідентифікатор> { ',' <ідентифікатор> }
ASTNode* Avariable_list()
{
    Amatch(Identifier);
    ASTNode* id = createNode(id_node, TokenTable[pos - 1].name, NULL, NULL);
    ASTNode* list = list = createNode(var_node, "Variable ", id, NULL);
    while (TokenTable[pos].type == Comma)
    {
        Amatch(Comma);
        Amatch(Identifier);
        id = createNode(id_node, TokenTable[pos - 1].name, NULL, NULL);
        list = createNode(var_node, "Variable ", id, list);
    }
    return list;
}
// <тіло програми> = <оператор> ';' { <оператор> ';' }
ASTNode* Aprogram_body()
{
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    ASTNode* stmt = Astatement();
    Amatch(Semicolon);
    ASTNode* body = stmt;
    while (TokenTable[pos].type != StopProgram)
    {
        ASTNode* nextStmt = Astatement();
        Amatch(Semicolon);
        body = createNode(statement_node, "statement", body, nextStmt);
    }
    return body;
}
// <оператор> = <присвоєння> | <ввід> | <вивід> | <умовний оператор> | <складений оператор>
ASTNode* Astatement()
{
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    switch (TokenTable[pos].type)
    {
    case Input: return Ainput();
    case Output: return Aoutput();
    case If: 
        printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
        printf("CURRENT ALGORITHM IS NOT LINEAR. CAN NOT CREATE ABSTRACT SYNTAX TREE. \n");
        exit(0);
        //return Aconditional();
    case StartProgram: return Acompound_statement();
    default: return Aassignment();
    }
}
// <присвоєння> = <ідентифікатор> '>>' <арифметичний вираз>
ASTNode* Aassignment()
{
    ASTNode* id = createNode(id_node, TokenTable[pos].name, NULL, NULL);
    Amatch(Identifier);
    Amatch(Assign);
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    if (TokenTable[pos].type == 11 || TokenTable[pos].type == Sub)
    {
        Amatch(Sub);
        ASTNode* expr = Aarithmetic_expression();
        return createNode(assign_node, ">>", id, expr);

    }
    else
    {
        ASTNode* expr = Aarithmetic_expression();
        return createNode(assign_node, ">>", id, expr);

    }
}
// <арифметичний вираз> = <доданок> { ('+' | '-') <доданок> }
ASTNode* Aarithmetic_expression()
{
    ASTNode* left = Aterm();
    while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub)
    {
        TypeOfTokens op = TokenTable[pos].type;
        Amatch(op);
        ASTNode* right = Aterm();
        if (op == Add)
            left = createNode(add_node, "+", left, right);
        else
            left = createNode(sub_node, "-", left, right);
    }
    return left;
}
// <доданок> = <множник> { ('*' | '/') <множник> }
ASTNode* Aterm()
{
    ASTNode* left = Afactor();
    while (TokenTable[pos].type == Mul || TokenTable[pos].type == Div ||TokenTable[pos].type == Mod)
    {
        TypeOfTokens op = TokenTable[pos].type;
        Amatch(op);
        ASTNode* right = Afactor();
        if (op == Mul)
            left = createNode(mul_node, "*", left, right);
        else  if (op == Div)
            left = createNode(div_node, "/", left, right);
        else  if (op == Mod)
            left = createNode(div_node, "%", left, right);
    }
    return left;
}
// <множник> = <ідентифікатор> | <число> | '(' <арифметичний вираз> ')'
ASTNode* Afactor()
{
    if (TokenTable[pos].type == Identifier)
    {
        ASTNode* id = createNode(id_node, TokenTable[pos].name, NULL, NULL);
        Amatch(Identifier);
        return id;
    }
    else
        if (TokenTable[pos].type == Number)
        {
            ASTNode* num = createNode(num_node, TokenTable[pos].name, NULL, NULL);
           Amatch(Number);
            return num;
        }
        else
            if (TokenTable[pos].type == LBraket)
            {
                Amatch(LBraket);
                ASTNode* expr = Aarithmetic_expression();
                Amatch(RBraket);
                return expr;
            }
            else
            {
                printf("\nSyntax error in line %d: A multiplier was expected.\n", TokenTable[pos].line);
                exit(11);
            }
}
// <ввід> = 'input' <ідентифікатор>
ASTNode* Ainput()
{
    Amatch(Input);
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    ASTNode* id = createNode(id_node, TokenTable[pos].name, NULL, NULL);
    //printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    Amatch(Identifier); 
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    return createNode(input_node, "input", id, NULL);
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
}
// <вивід> = 'output' <ідентифікатор>
ASTNode* Aoutput()
{
    Amatch(Output);
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    ASTNode* id = createNode(id_node, TokenTable[pos].name, NULL, NULL);
   // printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    Aarithmetic_expression();
  //  printf("Current token: %s, type: %d\n", TokenTable[pos].name, TokenTable[pos].type);
    return createNode(output_node, "output", id, NULL);
}
// <умовний оператор> = 'if' <логічний вираз> 'then' <оператор> [ 'else' <оператор> ]
ASTNode* Aconditional()
{
    Amatch(If);
    ASTNode* condition = Alogical_expression();
    Amatch(Then);
    ASTNode* ifBranch = Astatement();
    ASTNode* elseBranch = NULL;
    if (TokenTable[pos].type == Then)
    {
        Amatch(Then);
        elseBranch = Astatement();
    }
    return createNode(if_node, "if", condition, createNode(then_node, "then", ifBranch, elseBranch));
}
// <логічний вираз> = <вираз І> { '|' <вираз І> }
ASTNode* Alogical_expression()
{
    ASTNode* left = Aand_expression();
    while (TokenTable[pos].type == Or)
    {
        Amatch(Or);
        ASTNode* right = Aand_expression();
        left = createNode(or_node, "|", left, right);
    }
    return left;
}
// <вираз І> = <порівняння> { '&' <порівняння> }
ASTNode* Aand_expression()
{
    ASTNode* left = Acomparison();
    while (TokenTable[pos].type == And)
    {
        Amatch(And);
        ASTNode* right = Acomparison();
        left = createNode(and_node, "&", left, right);
    }
    return left;
}
// <порівняння> = <операція порівняння> | ‘!‘ ‘(‘ <логічний вираз> ‘)‘ | ‘(‘ <логічний вираз> ‘)‘
// <операція порівняння> = <арифметичний вираз> <менше-більше> <арифметичний вираз>
// <менше-більше> = ‘>‘ | ‘<‘ | ‘=‘ | ‘<>‘
ASTNode* Acomparison()
{
    if (TokenTable[pos].type == Not)
    {
        // Варіант: ! (<логічний вираз>)
        Amatch(Not);
        Amatch(LBraket);
        ASTNode* expr = Alogical_expression();
        Amatch(RBraket);
        return createNode(not_node, "!", expr, NULL);
    }
    else
        if (TokenTable[pos].type == LBraket)
        {
            // Варіант: ( <логічний вираз> )
            Amatch(LBraket);
            ASTNode* expr = Alogical_expression();
            Amatch(RBraket);
            return expr;  // Повертаємо вираз у дужках як піддерево
        }
        else
        {
            // Варіант: <арифметичний вираз> <менше-більше> <арифметичний вираз>
            ASTNode* left = Aarithmetic_expression();
            if (TokenTable[pos].type == NotGreater || TokenTable[pos].type == NotLess ||
                TokenTable[pos].type == Equality || TokenTable[pos].type == NotEquality)
            {
                TypeOfTokens op = TokenTable[pos].type;
                char operatorName[16];
                strcpy_s(operatorName, TokenTable[pos].name);
                Amatch(op);
                ASTNode* right = Aarithmetic_expression();
                return createNode(cmp_node, operatorName, left, right);
            }
            else
            {
                printf("\nSyntax error: A comparison operation is expected.\n");
                exit(12);
            }
        }
}
// <складений оператор> = 'start' <тіло програми> 'stop'
ASTNode* Acompound_statement()
{
    Amatch(StartProgram);
    ASTNode* body = Aprogram_body();
    Amatch(StopProgram);
    return createNode(compount_node, "compound", body, NULL);
}