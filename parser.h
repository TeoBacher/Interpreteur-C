#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR
} VariableType;


// Types of AST nodes
typedef enum
{
    NumberNode,
    IdentifierNode,
    BinaryOpNode,
    AssignmentNode,
    PrintNode,
    IfNode,
    BlockNode,
} ASTNodeType;

// Structure of an AST node
typedef struct ASTNode
{
    ASTNodeType nodeType;       // Type of AST node
    TokenType tokenType;        // For operators
    VariableType varType;
    int value;                  // For numbers
    char identifier[256];       // For variables
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *condition;
    struct ASTNode *thenBranch;
    struct ASTNode *elseBranch;
    struct ASTNode *next;
} ASTNode;

// Parser functions
ASTNode *parseProgram();
void freeAST(ASTNode *node);
int evaluateAST(ASTNode *node);
void evaluateProgram(ASTNode *node);

// Symbol table
typedef struct
{
    char identifier[256];
    VariableType type;
    int intValue;       
    float floatValue;   
    char charValue;
} SymbolTableEntry;



extern SymbolTableEntry symbolTable[100];
extern int symbolCount;

#endif