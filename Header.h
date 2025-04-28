#pragma once
#define MAX_TOKENS 1000
#define MAX_IDENTIFIER 8
// перерахування, яке описує всі можливі типи лексем
enum TypeOfTokens
{
    ProgramName,     // #Program
    StartProgram,       // Start
    StopProgram,	    // Stop
    Variable,		        // Variable
    Type,   		            // Integer32_t
    Identifier,	            // Identifier
    Number,		        // number

    Input,			        // Put
    Output,			        // Get

    Assign,                 // >> (присваивание)
    Add,                     // +
    Sub,                     // -
    Mul,                     // Mul
    Div,                      // Div
    Mod,                    // Mod

    Equality,               // =
    NotEquality,         // <>
    NotGreater,          // !>
    NotLess,               // !<

    Not,                     // !
    And,                    // And
    Or,                      // Or

    If,	    	               // If
    Then,	                   // Then 
    Goto,                    // Goto
    For,                      // For
    To,                       // To
    Next,                   // Next
    Step,

    LBraket,	        	// (
    RBraket,		        // )
    Semicolon,		    // ;
    Comma,			    // ,
    comment,
    Unknown             // Неведомая лексема
};
// структура для зберігання інформації про лексему
struct Token
{
    char name[16];      // ім'я лексеми
    int value;          // значення лексеми (для цілих констант)
    int line;           // номер рядка
    TypeOfTokens type;  // тип лексеми
};
// структура для зберігання інформації про ідентифікатор
struct Id
{
    char name[16];
};
// перерахування, яке описує стани лексичного аналізатора
enum class States
{
    Start,      // початок виділення чергової лексеми    
    Finish,     // кінець виділення чергової лексеми
    Letter,     // опрацювання слів (ключові слова і ідентифікатори)
    Digit,      // опрацювання цифри
    Separators, // видалення пробілів, символів табуляції і переходу на новий рядок
    Another,    // опрацювання інших символів
    EndOfFile,  // кінець файлу
    SComment,   // початок коментаря
    Comment,    // видалення коментаря
    ProgName    // початок програми
};
//
// перерахування, яке описує всі можливі вузли абстрактного синтаксичного дерева
//
enum TypeOfNodes
{
    program_node,
    var_node,
    input_node,
    output_node,
    if_node,
    then_node,
    id_node,
    num_node,
    assign_node,
    add_node,
    sub_node,
    mul_node,
    div_node,
    or_node,
    and_node,
    not_node,
    cmp_node,
    statement_node,
    compount_node
};
// структура, яка описує вузол абстрактного синтаксичного дерева (AST)
struct ASTNode
{
    TypeOfNodes nodetype;    // Тип вузла
    char name[16];           // Ім'я вузла
    struct ASTNode* left;    // Лівий нащадок
    struct ASTNode* right;   // Правий нащадок
};
//
// функція отримує лексеми з вхідного файлу F і записує їх у таблицю лексем TokenTable; результат функції - кількість лексем
//
unsigned int GetTokens(FILE* F, Token TokenTable[], FILE* logFile);
// функція друкує таблицю лексем на екран
void PrintTokens(Token TokenTable[], unsigned int TokensNum);
// функція друкує таблицю лексем у файл
void PrintTokensToFile(char* FileName, Token TokenTable[], unsigned int TokensNum);
//
// синтаксичний аналіз методом рекурсивного спуску
// вхідні дані - глобальна таблиця лексем TokenTable
//
void Parser(FILE* logFile);
void program(FILE* logFile);
void variable_declaration(FILE* logFile);
void variable_list(FILE* logFile);
void program_body(FILE* logFile);
void statement(FILE* logFile);
void assignment(FILE* logFile);
void arithmetic_expression(FILE* logFile);
void term(FILE* logFile);
void factor(FILE* logFile);
void input(FILE* logFile);
void output(FILE* logFile);
void GoTo(FILE* logFile);
void process_label(FILE* logFile);
void conditional(FILE* logFile);
void logical_expression(FILE* logFile);
void and_expression(FILE* logFile);
void comparison(FILE* logFile);
void ThenGoTo(FILE* logFile);
void For_loop(FILE* logFile);
void initial_value(FILE* logFile);
void loop_body(FILE* logFile);
void step(FILE* logFile);
void compound_statement(FILE* logFile);
unsigned int IdIdentification(Id IdTable[], Token TokenTable[], unsigned int tokenCount, FILE* logFile);
//
