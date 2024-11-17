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
const char *tokenTypeToString(TokenType type);
ASTNode *parseStatement();
ASTNode *parseBlock();
ASTNode *parseExpression();
ASTNode *parseTerm();
ASTNode *parseFactor();
ASTNode *parseIfStatement();
ASTNode *parseForStatement();
ASTNode *parseWhileStatement();
ASTNode *parseAssignment(VariableType varType);
ASTNode *parsePrintStatement();
int lookupVariable(const char *name);

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
    if (DEBUG)
        printf("Parser: Next token is '%s' of type %d\n", currentToken.value, currentToken.type);
}

// Match the current token with the expected token
void match(TokenType expected)
{
    if (currentToken.type == expected)
    {
        if (DEBUG)
            printf("Parser: Matched token '%s'\n", currentToken.value);
        nextToken();
    }
    else
    {
        printf("Syntax Error: Expected token type %d, but got %d\n", expected, currentToken.type);
        exit(1);
    }
}

// Look up a symbol in the symbol table
SymbolTableEntry *lookupSymbol(const char *name)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].identifier, name) == 0)
        {
            return &symbolTable[i];
        }
    }
    return NULL;
}

ASTNode *parseProgram()
{
    if (DEBUG)
        printf("Parser: Starting to parse program\n");
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

    if (DEBUG)
        printf("Parser: Finished parsing program\n");
    return statements;
}

ASTNode *parseStatement()
{
    if (DEBUG)
        printf("Parser: Parsing a statement\n");
    ASTNode *node;

    switch (currentToken.type)
    {
    case IntKeyword:
        nextToken();
        node = parseAssignment(TYPE_INT);
        match(Semicolon);
        break;
    case CharKeyword:
        nextToken();
        node = parseAssignment(TYPE_CHAR);
        match(Semicolon);
        break;
    case If:
        node = parseIfStatement();
        break;
    case For:
        node = parseForStatement();
        break;
    case While:
        node = parseWhileStatement();
        break;
    case Print:
        node = parsePrintStatement();
        match(Semicolon);
        break;
    case Identifier:
        node = parseAssignment( TYPE_INT);
        match(Semicolon);
        break;
    default:
        printf("Syntax Error: Unexpected token '%s' of type %d\n", currentToken.value, currentToken.type);
        exit(1);
        break;
    }

    return node;
}

ASTNode *parseIfStatement()
{
    if (DEBUG)
        printf("Parser: Parsing an if statement\n");
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
            if (DEBUG)
                printf("Parser: Parsing else if clause\n");
            match(ElseIf);
            node->elseBranch = parseIfStatement();
        }
        else
        {
            if (DEBUG)
                printf("Parser: Parsing else clause\n");
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

ASTNode *parseForStatement()
{
    if (DEBUG)
        printf("Parser: Parsing a for statement\n");
    ASTNode *node = malloc(sizeof(ASTNode));
    node->nodeType = ForNode;

    match(For);
    match(Lparen);

    // Ini
    node->init = parseAssignment( TYPE_INT);
    match(Semicolon);

    // Condition
    node->condition = parseExpression();
    match(Semicolon);

    // Increment
    node->increment = parseAssignment( TYPE_INT);
    match(Rparen);

    node->body = parseBlock();

    return node;
}

ASTNode *parseWhileStatement()
{
    if (DEBUG)
        printf("Parser: Parsing a while statement\n");
    ASTNode *node = malloc(sizeof(ASTNode));
    node->nodeType = WhileNode;

    match(While);
    match(Lparen);
    node->condition = parseExpression();
    match(Rparen);

    node->body = parseBlock();

    return node;
}

ASTNode *parseBlock()
{
    if (DEBUG)
        printf("Parser: Parsing a block\n");
    match(Lbrace); // '{'
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

    match(Rbrace); // '}'
    return statements;
}

// Assignation for int
void assignVariableInt(const char *name, VariableType type, int intValue)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].identifier, name) == 0)
        {
            symbolTable[i].type = type;
            symbolTable[i].intValue = intValue;
            return;
        }
    }
    if (symbolCount < 100)
    {
        strcpy(symbolTable[symbolCount].identifier, name);
        symbolTable[symbolCount].type = type;
        symbolTable[symbolCount].intValue = intValue;
        symbolCount++;
    }
    else
    {
        printf("Runtime Error: Symbol table full\n");
        exit(1);
    }
}

//  Assignation for string
void assignVariableString(const char *name, VariableType type, const char *value) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].identifier, name) == 0) {
            symbolTable[i].type = type;
            strncpy(symbolTable[i].charValue, value, sizeof(symbolTable[i].charValue) - 1);
            symbolTable[i].charValue[sizeof(symbolTable[i].charValue) - 1] = '\0';  // Sécurise la fin de chaîne
            return;
        }
    }

    if (symbolCount < 100) {
        strcpy(symbolTable[symbolCount].identifier, name);
        symbolTable[symbolCount].type = type;
        strncpy(symbolTable[symbolCount].charValue, value, sizeof(symbolTable[symbolCount].charValue) - 1);
        symbolTable[symbolCount].charValue[sizeof(symbolTable[symbolCount].charValue) - 1] = '\0';
        symbolCount++;
    } else {
        printf("Runtime Error: Symbol table full\n");
        exit(1);
    }
}


ASTNode *parseAssignment(VariableType varType)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->nodeType = AssignmentNode;
    node->varType = varType;

    // Vérifie si le token actuel est un identifiant
    if (currentToken.type == Identifier) {
        strcpy(node->identifier, currentToken.value);  
        match(Identifier);  
    } else {
        printf("Syntax Error: Expected an identifier, but got '%s'\n", currentToken.value);
        exit(1);
    }

    // Vérifie si le prochain token est '='
    if (currentToken.type == Assign) {
        match(Assign);  
    } else {
        printf("Syntax Error: Expected '=', but got '%s'\n", currentToken.value);
        exit(1);
    }

    // Gère l'expression après l'affectation
    if (varType == TYPE_CHAR && currentToken.type == StringLiteral) {
        node->right = malloc(sizeof(ASTNode));
        node->right->nodeType = CharLiteralNode;
        node->right->stringValue = strdup(currentToken.value);  // Copie la chaîne
        match(StringLiteral);
    } else {
        node->right = parseExpression();  // Pour d'autres types
    }

    return node;
}


ASTNode *parsePrintStatement()
{
    if (DEBUG)
        printf("Parser: Parsing a print statement\n");
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
    if (DEBUG)
        printf("Parser: Parsing an expression\n");
    ASTNode *node = parseTerm();

    // Parse binary operators
    while (currentToken.type == Add || currentToken.type == Sub ||
           currentToken.type == Lt || currentToken.type == Le ||
           currentToken.type == Gt || currentToken.type == Ge ||
           currentToken.type == Ne)
    {
        if (DEBUG)
            printf("Parser: Parsing binary operator '%s'\n", currentToken.value);
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
    if (DEBUG)
        printf("Parser: Parsing a term\n");
    ASTNode *node = parseFactor();

    while (currentToken.type == Mul || currentToken.type == Div || currentToken.type == Mod)
    {
        if (DEBUG)
            printf("Parser: Parsing binary operator '%s'\n", currentToken.value);
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
    if (DEBUG)
        printf("Parser: Parsing a factor\n");
    ASTNode *node;

    if (currentToken.type == Number)
    {
        if (DEBUG)
            printf("Parser: Recognized number '%s'\n", currentToken.value);
        node = malloc(sizeof(ASTNode));
        node->nodeType = NumberNode;
        node->value = atoi(currentToken.value);
        match(Number);
    }
    else if (currentToken.type == Identifier)
    {
        if (DEBUG)
            printf("Parser: Recognized identifier '%s'\n", currentToken.value);
        node = malloc(sizeof(ASTNode));
        node->nodeType = IdentifierNode;
        strcpy(node->identifier, currentToken.value);
        match(Identifier);
    }
    else if (currentToken.type == StringLiteral)
    {
        if (DEBUG)
            printf("Parser: Recognized string literal '%s'\n", currentToken.value);

        node = malloc(sizeof(ASTNode));
        node->nodeType = CharLiteralNode;

        node->stringValue = malloc(strlen(currentToken.value) + 1);
        strcpy(node->stringValue, currentToken.value);
        match(StringLiteral);
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

const char *tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case Semicolon:
        return ";";
    case Lparen:
        return "(";
    case Rparen:
        return ")";
    case Identifier:
        return "identifier";
    case Number:
        return "number";
    case Assign:
        return "=";
    default:
        return "token";
    }
}

int evaluateAST(ASTNode *node)
{
    if (node == NULL)
    {
        return 0;
    }

    // print all the nodes
    if (DEBUG)
    {
        switch (node->nodeType)
        {
        case NumberNode:
            printf("Evaluator: Number node with value %d\n", node->value);
            break;
        case IdentifierNode:
            printf("Evaluator: Identifier '%s'\n", node->identifier);
            break;
        case CharLiteralNode:
            printf("Evaluator: Char literal with value '%c'\n", node->charValue);
            break;
        case BinaryOpNode:
            printf("Evaluator: Binary operator '%d'\n", node->tokenType);
            break;
        case AssignmentNode:
            printf("Evaluator: Assignment to '%s'\n", node->identifier);
            break;
        case PrintNode:
            printf("Evaluator: Print statement\n");
            break;
        case IfNode:
            printf("Evaluator: If statement\n");
            break;
        case ForNode:
            printf("Evaluator: For loop\n");
            break;
        case WhileNode:
            printf("Evaluator: While loop\n");
            break;
        default:
            printf("Evaluator: Unknown node type: %d\n", node->nodeType);
            exit(1);
            break;
        }
    }

    switch (node->nodeType)
    {
    case NumberNode:
        if (DEBUG)
            printf("Evaluator: Number node with value %d\n", node->value);
        return node->value;

    case IdentifierNode:
    {
        int value = lookupVariable(node->identifier);
        if (DEBUG)
            printf("Evaluator: Identifier '%s' has value %d\n", node->identifier, value);
        return value;
    }
    case CharLiteralNode:
        if (DEBUG)
            printf("Evaluator: Char literal with value '%c'\n", node->charValue);
        return node->charValue;

    case BinaryOpNode:
    {
        int leftValue = evaluateAST(node->left);
        int rightValue = evaluateAST(node->right);
        int result;
        if (DEBUG)
            printf("Evaluator: Performing binary operation '%d' on %d and %d\n", node->tokenType, leftValue, rightValue);
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
        if (node->varType == TYPE_INT)
        {
            int value = evaluateAST(node->right);
            assignVariableInt(node->identifier, node->varType, value);
            if (DEBUG)
                printf("Evaluator: Assigned int value %d to variable '%s'\n", value, node->identifier);
            return value;
        }
        else if (node->varType == TYPE_CHAR)
        {
            if (node->right->nodeType == CharLiteralNode)
            {
                const char *stringValue = node->right->stringValue;
                assignVariableString(node->identifier, node->varType, stringValue);
                if (DEBUG)
                    printf("Evaluator: Assigned string value '%s' to variable '%s'\n", stringValue, node->identifier);
                return 0;
            }
            else
            {
                printf("Runtime Error: Unexpected type for variable '%s'\n", node->identifier);
                exit(1);
            }
        }

        return 0;
    }

    case IfNode:
    {
        int conditionResult = evaluateAST(node->condition);
        if (DEBUG)
            printf("Evaluator: If condition evaluated to %d\n", conditionResult);
        if (conditionResult)
        {
            if (DEBUG)
                printf("Evaluator: Executing 'then' branch\n");
            evaluateAST(node->thenBranch);
        }
        else if (node->elseBranch != NULL)
        {
            if (DEBUG)
                printf("Evaluator: Executing 'else' branch\n");
            evaluateAST(node->elseBranch);
        }
        return 0;
    }

    case ForNode:
    {
        if (DEBUG)
            printf("Evaluator: Evaluating a for loop\n");
        for (
            evaluateAST(node->init);
            evaluateAST(node->condition);
            evaluateAST(node->increment))
        {
            evaluateAST(node->body);
        }
        return 0;
    }

    case WhileNode:
    {
        if (DEBUG)
            printf("Evaluator: Evaluating a while loop\n");
        while (evaluateAST(node->condition))
        {
            ASTNode *bodyNode = node->body;
            while (bodyNode != NULL)
            {
                evaluateAST(bodyNode);
                bodyNode = bodyNode->next;
            }
        }
        break;
    }

    case PrintNode:
    {
        SymbolTableEntry *entry = lookupSymbol(node->left->identifier);
        if (entry != NULL)
        {
            if (entry->type == TYPE_INT)
            {
                printf("%d\n", entry->intValue);
            }
            else
            {
                printf("%s\n", entry->charValue);
            }
        }
        return 0;
    }

    default:
    {
        printf("Runtime Error: Unknown AST node type '%d'\n", node->nodeType);
        exit(1);
    }
    }

    // Evaluate the next statement unless, it's a loop
    if (node->nodeType != WhileNode && node->nodeType != ForNode && node->next != NULL)
    {
        evaluateAST(node->next);
    }

    return 0;
}

int lookupVariable(const char *name)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].identifier, name) == 0)
        {
            if (symbolTable[i].type != TYPE_INT)
            {
                printf("Type Error: Variable '%s' is not of type int\n", name);
                exit(1);
            }
            return symbolTable[i].intValue;
        }
    }
    printf("Runtime Error: Undefined variable '%s'\n", name);
    exit(1);
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
    freeAST(node->body);
    freeAST(node->init);
    freeAST(node->increment);
    freeAST(node->next);

    free(node);
}