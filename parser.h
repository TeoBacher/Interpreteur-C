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
    CharLiteralNode,
    StringLiteralNode
} ASTNodeType;

// Structure of an AST node
typedef struct ASTNode
{
    ASTNodeType nodeType;       // Type of AST node
    TokenType tokenType;        // For operators
    VariableType varType;
    int value;                  // For numbers
    char identifier[256];       // For variables
    char charValue;
    char *stringValue;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *condition;
    struct ASTNode *thenBranch;
    struct ASTNode *elseBranch;
    struct ASTNode *next;
} ASTNode;

// Parser functions
ASTNode *parseAssignment(VariableType varType);
ASTNode *parseProgram();
void freeAST(ASTNode *node);
int evaluateAST(ASTNode *node);
void evaluateProgram(ASTNode *node);

// Assign Variable
void assignVariableString(const char *name, VariableType type, const char *value);
void assignVariableInt(const char *name, VariableType type, int intValue);

// Symbol table
typedef struct
{
    char identifier[256];
    VariableType type;

    union {
        int intValue;
        char charValue[256];  
        float floatValue;
    };
} SymbolTableEntry;


SymbolTableEntry *lookupSymbol(const char *name);
extern SymbolTableEntry symbolTable[100];
extern int symbolCount;

#endif