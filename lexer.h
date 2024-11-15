#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enable debugging by setting DEBUG to 1
#define DEBUG 1

// Definition of token types
typedef enum
{
    // Single-character tokens
    Number = 0,     // Numbers
    Identifier = 1, // Identifiers
    Add = 2,        // '+'
    Sub = 3,        // '-'
    Mul = 4,        // '*'
    Div = 5,        // '/'
    Mod = 6,        // '%'
    Inc = 7,        // '++' // TODO
    Dec = 8,        // '--' // TODO
    Pow = 9,        // '^' // TODO
    Lt = 10,        // '<'
    Le = 11,        // '<='
    Gt = 12,        // '>'
    Ge = 13,        // '>='
    Ne = 14,        // '!='
    Assign = 15,    // '='
                    // Keywords
    If = 16,        // 'if'
    Else = 17,      // 'else'
    ElseIf = 18,    // 'else if'
    For = 19,       // 'for'
    While = 20,     // 'while'
    Print = 21,     // 'print'
                    // Symbols
    Lparen = 22,    // '('
    Rparen = 23,    // ')'
    Lbrace = 24,    // '{'
    Rbrace = 25,    // '}'
    Semicolon = 26, // ';'
                    // Special tokens
    Eof = 27,       // End of file
    Error = 28      // Error
} TokenType;

// Token structure
typedef struct
{
    TokenType type; // Type of token
    char *value;    // Value of token
} Token;

// Lexer functions
void setInput(const char *inputStr);
Token getNextToken();
void freeToken(Token token);
char peek();
void advance();
void skipWhitespace();
Token createToken(TokenType type, const char *value);
Token peekNextToken();

#endif