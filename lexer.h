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
    Inc = 7,        // '++'
    Dec = 8,        // '--'
    Pow = 9,        // '^'
    Lt = 10,        // '<'
    Le = 11,        // '<='
    Gt = 12,        // '>'
    Ge = 13,        // '>='
    Ne = 14,        // '!='
    Assign = 15,    // '='
                 // Keywords
    If = 16,          // 'if'
    Else = 17,        // 'else'
    ElseIf = 18,      // 'else if'
    Print = 19,       // 'print'
    CharKeyword = 20, // 'char'
    IntKeyword = 21,  // 'int'
    For = 22,         // 'for'
    While = 23,       // 'while'
                // Literals
    CharLiteral = 24, // literal value, ex 'a'
    StringLiteral = 25,

    // Symbols
    Lparen = 26,    // '('
    Rparen = 27,    // ')'
    Lbrace = 28,    // '{'
    Rbrace = 29,    // '}'
    Semicolon = 30, // ';'
                    // Special tokens
    Eof = 31,  // End of file
    Error = 32 // Error
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