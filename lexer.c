#include "lexer.h"
#include <ctype.h>

static const char *input; // Input string
static int position = 0;  // Current position in input

void setInput(const char *inputStr)
{
    input = inputStr;
    position = 0;
}

char peek()
{
    return input[position];
}

void advance()
{
    if (peek() != '\0')
    {
        position++;
    }
}

void skipWhitespace()
{
    while (isspace(peek()))
    {
        advance();
    }
}

Token createToken(TokenType type, const char *value)
{
    Token token;
    token.type = type;
    token.value = strdup(value);
    return token;
}

Token getNextToken()
{
    skipWhitespace();

    char current_char = peek();

    // End of input
    if (current_char == '\0')
    {
        if (DEBUG) printf("Lexer: End of input\n");
        return createToken(Eof, "");
    }

    // Numbers
    if (isdigit(current_char))
    {
        char buffer[256];
        int i = 0;
        while (isdigit(peek()))
        {
            buffer[i++] = peek();
            advance();
        }
        buffer[i] = '\0';
        if (DEBUG) printf("Lexer: Recognized number '%s'\n", buffer);
        return createToken(Number, buffer);
    }

    // Identifiers and keywords
    if (isalpha(current_char))
    {
        char buffer[256];
        int i = 0;
        while (isalnum(peek()) || peek() == '_')
        {
            buffer[i++] = peek();
            advance();
        }
        buffer[i] = '\0';

        // Keywords
        if (strcmp(buffer, "if") == 0)
        {
            if (DEBUG) printf("Lexer: Recognized keyword 'if'\n");
            return createToken(If, "if");
        }
        else if (strcmp(buffer, "else") == 0)
        {
            // Check for 'else if'
            skipWhitespace();
            if (strncmp(&input[position], "if", 2) == 0 && !isalnum(input[position + 2]))
            {
                // Recognize 'else if' as a single token
                advance(); // 'i'
                advance(); // 'f'
                if (DEBUG) printf("Lexer: Recognized keyword 'else if'\n");
                return createToken(ElseIf, "else if");
            }
            else
            {
                if (DEBUG) printf("Lexer: Recognized keyword 'else'\n");
                return createToken(Else, "else");
            }
        }
        else if (strcmp(buffer, "print") == 0)
        {
            if (DEBUG) printf("Lexer: Recognized keyword 'print'\n");
            return createToken(Print, "print");
        }
        else
        {
            if (DEBUG) printf("Lexer: Recognized identifier '%s'\n", buffer);
            return createToken(Identifier, buffer);
        }
    }

    // Operators and symbols
    switch (current_char)
    {
    case '+':
        advance();
        if (peek() == '+')
        {
            advance();
            if (DEBUG) printf("Lexer: Recognized operator '++'\n");
            return createToken(Inc, "++");
        }
        else
        {
            if (DEBUG) printf("Lexer: Recognized operator '+'\n");
            return createToken(Add, "+");
        }
    case '-':
        advance();
        if (peek() == '-')
        {
            advance();
            if (DEBUG) printf("Lexer: Recognized operator '--'\n");
            return createToken(Dec, "--");
        }
        else
        {
            if (DEBUG) printf("Lexer: Recognized operator '-'\n");
            return createToken(Sub, "-");
        }
    case '*':
        advance();
        if (DEBUG) printf("Lexer: Recognized operator '*'\n");
        return createToken(Mul, "*");
    case '/':
        advance();
        if (DEBUG) printf("Lexer: Recognized operator '/'\n");
        return createToken(Div, "/");
    case '%':
        advance();
        if (DEBUG) printf("Lexer: Recognized operator '%%'\n");
        return createToken(Mod, "%");
    case '^':
        advance();
        if (DEBUG) printf("Lexer: Recognized operator '^'\n");
        return createToken(Pow, "^");
    case '=':
        advance();
        if (DEBUG) printf("Lexer: Recognized operator '='\n");
        return createToken(Assign, "=");
    case '<':
        advance();
        if (peek() == '=')
        {
            advance();
            if (DEBUG) printf("Lexer: Recognized operator '<='\n");
            return createToken(Le, "<=");
        }
        else
        {
            if (DEBUG) printf("Lexer: Recognized operator '<'\n");
            return createToken(Lt, "<");
        }
    case '>':
        advance();
        if (peek() == '=')
        {
            advance();
            if (DEBUG) printf("Lexer: Recognized operator '>='\n");
            return createToken(Ge, ">=");
        }
        else
        {
            if (DEBUG) printf("Lexer: Recognized operator '>'\n");
            return createToken(Gt, ">");
        }
    case '!':
        advance();
        if (peek() == '=')
        {
            advance();
            if (DEBUG) printf("Lexer: Recognized operator '!='\n");
            return createToken(Ne, "!=");
        }
        else
        {
            printf("Lexer Error: Unexpected character '!' without '='\n");
            exit(1);
        }
    case '(':
        advance();
        if (DEBUG) printf("Lexer: Recognized symbol '('\n");
        return createToken(Lparen, "(");
    case ')':
        advance();
        if (DEBUG) printf("Lexer: Recognized symbol ')'\n");
        return createToken(Rparen, ")");
    case '{':
        advance();
        if (DEBUG) printf("Lexer: Recognized symbol '{'\n");
        return createToken(Lbrace, "{");
    case '}':
        advance();
        if (DEBUG) printf("Lexer: Recognized symbol '}'\n");
        return createToken(Rbrace, "}");
    default:
        printf("Lexer Error: Unknown character '%c'\n", current_char);
        exit(1);
    }
}

void freeToken(Token token)
{
    free(token.value);
}