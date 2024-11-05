#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Current token
Token currentToken;

// Symbol table
SymbolTableEntry symbolTable[100];
int symbolCount = 0;

// Function prototypes
void nextToken();
void match(TokenType expected);
ASTNode *parseStatement();
ASTNode *parseBlock();
ASTNode *parseExpression();
ASTNode *parseTerm();
ASTNode *parseFactor();
ASTNode *parseIfStatement();
ASTNode *parseAssignment();
ASTNode *parsePrintStatement();
int lookupVariable(const char *name);
void assignVariable(const char *name, int value);

// Parser entry point
void evaluateProgram(ASTNode *node)
{
    while (node != NULL)
    {
        evaluateAST(node);
        node = node->next;
    }
}

void nextToken()
{
    currentToken = getNextToken();
    if (DEBUG) printf("Parser: Next token is '%s' of type %d\n", currentToken.value, currentToken.type);
}

// Match the current token with the expected token
void match(TokenType expected)
{
    if (currentToken.type == expected)
    {
        if (DEBUG) printf("Parser: Matched token '%s'\n", currentToken.value);
        nextToken();
    }
    else
    {
        printf("Syntax Error: Expected token type %d, but got %d\n", expected, currentToken.type);
        exit(1);
    }
}

ASTNode *parseProgram()
{
    if (DEBUG) printf("Parser: Starting to parse program\n");
    nextToken();
    ASTNode *statements = NULL;
    ASTNode *lastStatement = NULL;

    while (currentToken.type != Eof)
    {
        ASTNode *stmt = parseStatement();

        if (statements == NULL)
        {
            statements = stmt;
        }
        else
        {
            lastStatement->next = stmt;
        }
        lastStatement = stmt;
    }

    if (DEBUG) printf("Parser: Finished parsing program\n");
    return statements;
}

ASTNode *parseStatement()
{
    if (DEBUG) printf("Parser: Parsing a statement\n");
    ASTNode *node;

    if (currentToken.type == If)
    {
        node = parseIfStatement();
    }
    else if (currentToken.type == Print)
    {
        node = parsePrintStatement();
    }
    else if (currentToken.type == Identifier)
    {
        node = parseAssignment();
    }
    else
    {
        printf("Syntax Error: Unexpected token '%s' of type %d\n", currentToken.value, currentToken.type);
        exit(1);
    }

    return node;
}

ASTNode *parseIfStatement()
{
    if (DEBUG) printf("Parser: Parsing an if statement\n");
    ASTNode *node = malloc(sizeof(ASTNode));
    node->nodeType = IfNode;

    match(If);
    match(Lparen);
    node->condition = parseExpression();
    match(Rparen);

    node->thenBranch = parseBlock();

    if (currentToken.type == Else || currentToken.type == ElseIf)
    {
        if (currentToken.type == ElseIf)
        {
            if (DEBUG) printf("Parser: Parsing else if clause\n");
            match(ElseIf);
            node->elseBranch = parseIfStatement();
        }
        else
        {
            if (DEBUG) printf("Parser: Parsing else clause\n");
            match(Else);
            node->elseBranch = parseBlock();
        }
    }
    else
    {
        node->elseBranch = NULL;
    }

    return node;
}

ASTNode *parseBlock()
{
    if (DEBUG) printf("Parser: Parsing a block\n");
    match(Lbrace); // {
    ASTNode *statements = NULL;
    ASTNode *lastStatement = NULL;

    while (currentToken.type != Rbrace && currentToken.type != Eof)
    {
        ASTNode *stmt = parseStatement();

        if (statements == NULL)
        {
            statements = stmt;
        }
        else
        {
            lastStatement->next = stmt;
        }
        lastStatement = stmt;
    }

    match(Rbrace); // }
    return statements;
}

ASTNode *parseAssignment()
{
    if (DEBUG) printf("Parser: Parsing an assignment\n");
    ASTNode *node = malloc(sizeof(ASTNode));
    node->nodeType = AssignmentNode;
    strcpy(node->identifier, currentToken.value);
    match(Identifier);
    match(Assign);
    node->right = parseExpression();
    return node;
}

ASTNode *parsePrintStatement()
{
    if (DEBUG) printf("Parser: Parsing a print statement\n");
    ASTNode *node = malloc(sizeof(ASTNode));
    node->nodeType = PrintNode;
    match(Print);
    match(Lparen);
    node->left = parseExpression();
    match(Rparen);
    return node;
}

ASTNode *parseExpression()
{
    if (DEBUG) printf("Parser: Parsing an expression\n");
    ASTNode *node = parseTerm();

    // Parse binary operators
    while (currentToken.type == Add || currentToken.type == Sub ||
           currentToken.type == Lt || currentToken.type == Le ||
           currentToken.type == Gt || currentToken.type == Ge ||
           currentToken.type == Ne)
    {
        if (DEBUG) printf("Parser: Parsing binary operator '%s'\n", currentToken.value);
        ASTNode *temp = malloc(sizeof(ASTNode));
        temp->nodeType = BinaryOpNode;
        temp->tokenType = currentToken.type;
        temp->left = node;
        match(currentToken.type);
        temp->right = parseTerm();
        node = temp;
    }

    return node;
}

ASTNode *parseTerm()
{
    if (DEBUG) printf("Parser: Parsing a term\n");
    ASTNode *node = parseFactor();

    while (currentToken.type == Mul || currentToken.type == Div || currentToken.type == Mod)
    {
        if (DEBUG) printf("Parser: Parsing binary operator '%s'\n", currentToken.value);
        ASTNode *temp = malloc(sizeof(ASTNode));
        temp->nodeType = BinaryOpNode;
        temp->tokenType = currentToken.type;
        temp->left = node;
        match(currentToken.type);
        temp->right = parseFactor();
        node = temp;
    }

    return node;
}

ASTNode *parseFactor()
{
    if (DEBUG) printf("Parser: Parsing a factor\n");
    ASTNode *node;

    if (currentToken.type == Number)
    {
        if (DEBUG) printf("Parser: Recognized number '%s'\n", currentToken.value);
        node = malloc(sizeof(ASTNode));
        node->nodeType = NumberNode;
        node->value = atoi(currentToken.value);
        match(Number);
    }
    else if (currentToken.type == Identifier)
    {
        if (DEBUG) printf("Parser: Recognized identifier '%s'\n", currentToken.value);
        node = malloc(sizeof(ASTNode));
        node->nodeType = IdentifierNode;
        strcpy(node->identifier, currentToken.value);
        match(Identifier);
    }
    else if (currentToken.type == Lparen)
    {
        match(Lparen);
        node = parseExpression();
        match(Rparen);
    }
    else
    {
        printf("Syntax Error: Unexpected token '%s'\n", currentToken.value);
        exit(1);
    }

    return node;
}

int evaluateAST(ASTNode *node)
{
    if (node == NULL)
    {
        return 0;
    }

    switch (node->nodeType)
    {
    case NumberNode:
        if (DEBUG) printf("Evaluator: Number node with value %d\n", node->value);
        return node->value;

    case IdentifierNode:
    {
        int value = lookupVariable(node->identifier);
        if (DEBUG) printf("Evaluator: Identifier '%s' has value %d\n", node->identifier, value);
        return value;
    }

    case BinaryOpNode:
    {
        int leftValue = evaluateAST(node->left);
        int rightValue = evaluateAST(node->right);
        int result;
        if (DEBUG) printf("Evaluator: Performing binary operation '%d' on %d and %d\n", node->tokenType, leftValue, rightValue);
        switch (node->tokenType)
        {
            case Add:
                result = leftValue + rightValue;
                break;
            case Sub:
                result = leftValue - rightValue;
                break;
            case Mul:
                result = leftValue * rightValue;
                break;
            case Div:
                result = leftValue / rightValue;
                break;
            case Mod:
                result = leftValue % rightValue;
                break;
            case Lt:
                result = leftValue < rightValue;
                break;
            case Le:
                result = leftValue <= rightValue;
                break;
            case Gt:
                result = leftValue > rightValue;
                break;
            case Ge:
                result = leftValue >= rightValue;
                break;
            case Ne:
                result = leftValue != rightValue;
                break;
            default:
                printf("Runtime Error: Unknown binary operator '%d'\n", node->tokenType);
                exit(1);
        }
        return result;
    }

    case AssignmentNode:
    {
        int value = evaluateAST(node->right);
        assignVariable(node->identifier, value);
        if (DEBUG) printf("Evaluator: Assigned value %d to variable '%s'\n", value, node->identifier);
        return value;
    }

    case PrintNode:
    {
        int value = evaluateAST(node->left);
        printf("%d\n", value);
        if (DEBUG) printf("Evaluator: Printed value %d\n", value);
        return value;
    }

    case IfNode:
    {
        int conditionResult = evaluateAST(node->condition);
        if (DEBUG) printf("Evaluator: If condition evaluated to %d\n", conditionResult);
        if (conditionResult)
        {
            if (DEBUG) printf("Evaluator: Executing 'then' branch\n");
            evaluateAST(node->thenBranch);
        }
        else if (node->elseBranch != NULL)
        {
            if (DEBUG) printf("Evaluator: Executing 'else' branch\n");
            evaluateAST(node->elseBranch);
        }
        return 0;
    }

    default:
    {
        printf("Runtime Error: Unknown AST node type '%d'\n", node->nodeType);
        exit(1);
    }
    }
}

int lookupVariable(const char *name)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].identifier, name) == 0)
        {
            if (DEBUG) printf("Evaluator: Variable '%s' found with value %d\n", name, symbolTable[i].value);
            return symbolTable[i].value;
        }
    }
    printf("Runtime Error: Undefined variable '%s'\n", name);
    exit(1);
}

void assignVariable(const char *name, int value)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].identifier, name) == 0)
        {
            symbolTable[i].value = value;
            if (DEBUG) printf("Evaluator: Updated variable '%s' with new value %d\n", name, value);
            return;
        }
    }
    if (symbolCount < 100)
    {
        strcpy(symbolTable[symbolCount].identifier, name);
        symbolTable[symbolCount].value = value;
        symbolCount++;
        if (DEBUG) printf("Evaluator: Created new variable '%s' with value %d\n", name, value);
    }
    else
    {
        printf("Runtime Error: Symbol table full\n");
        exit(1);
    }
}

void freeAST(ASTNode *node)
{
    if (node == NULL)
        return;

    freeAST(node->left);
    freeAST(node->right);
    freeAST(node->condition);
    freeAST(node->thenBranch);
    freeAST(node->elseBranch);
    freeAST(node->next);

    free(node);
}