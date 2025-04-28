#pragma once
#define MAX_TOKENS 1000
#define MAX_IDENTIFIER 8
// �������������, ��� ����� �� ������ ���� ������
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

    Assign,                 // >> (������������)
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
    Unknown             // ��������� �������
};
// ��������� ��� ��������� ���������� ��� �������
struct Token
{
    char name[16];      // ��'� �������
    int value;          // �������� ������� (��� ����� ��������)
    int line;           // ����� �����
    TypeOfTokens type;  // ��� �������
};
// ��������� ��� ��������� ���������� ��� �������������
struct Id
{
    char name[16];
};
// �������������, ��� ����� ����� ���������� ����������
enum class States
{
    Start,      // ������� �������� ������� �������    
    Finish,     // ����� �������� ������� �������
    Letter,     // ����������� ��� (������ ����� � ��������������)
    Digit,      // ����������� �����
    Separators, // ��������� ������, ������� ��������� � �������� �� ����� �����
    Another,    // ����������� ����� �������
    EndOfFile,  // ����� �����
    SComment,   // ������� ���������
    Comment,    // ��������� ���������
    ProgName    // ������� ��������
};
//
// �������������, ��� ����� �� ������ ����� ������������ ������������� ������
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
// ���������, ��� ����� ����� ������������ ������������� ������ (AST)
struct ASTNode
{
    TypeOfNodes nodetype;    // ��� �����
    char name[16];           // ��'� �����
    struct ASTNode* left;    // ˳��� �������
    struct ASTNode* right;   // ������ �������
};
//
// ������� ������ ������� � �������� ����� F � ������ �� � ������� ������ TokenTable; ��������� ������� - ������� ������
//
unsigned int GetTokens(FILE* F, Token TokenTable[], FILE* logFile);
// ������� ����� ������� ������ �� �����
void PrintTokens(Token TokenTable[], unsigned int TokensNum);
// ������� ����� ������� ������ � ����
void PrintTokensToFile(char* FileName, Token TokenTable[], unsigned int TokensNum);
//
// ������������ ����� ������� ������������ ������
// ����� ��� - ��������� ������� ������ TokenTable
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
