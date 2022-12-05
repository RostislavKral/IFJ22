//
// Created by jeza on 30.11.22.
//
#include "stdlib.h"
#include "stdio.h"
#include "token.h"
void exit_with_message(int lineNum, int charNum, char* message, enum ERROR_CODE errorCode){
    fprintf(stderr, "ERR: line: %d, col: %d, message: %s\n\n", lineNum,charNum,message);
    switch (errorCode) {
        case LEXICAL_ERR:
            exit(1);
        case SYNTAX_ERR:
            exit(2);
        case SEM_F_DECLARATION_ERR:
            exit(3);
        case SEM_F_CALL_PARAM_ERR:
            exit(4);
        case SEM_UNDEF_VAR_ERR:
            exit(5);
        case SEM_F_RETURN_VAL_ERR:
            exit(6);
        case SEM_MATH_ERR:
            exit(7);
        case SEM_OTHER:
            exit(8);
        case GENERAL_ERR:
            exit(99);
    }
}

void token_print(TOKEN_T* token)
{
    if (token->type == TOKEN_ID)
    {
        printf("%s", token->name);
    }
    else if (token->type == LITERAL)
    {
        if (token->value.type == 0)
        {
            printf("%d", token->value.int_val);
        }
        else if (token->value.type == 1)
        {
            printf("%s", token->value.char_val);
        }
        else if (token->value.type == 2)
        {
            printf("%f", token->value.double_val);
        }
    }
    else if (token->type == DOLLAR)
    {
        printf("$");
    }
    else if (token->type == LPAR)
    {
        printf("(");
    }
    else if (token->type == RPAR)
    {
        printf(")");
    }
    else if (token->operators == MULTIPLY)
    {
        printf("*");
    }
    else if (token->operators == DIVIDE)
    {
        printf("/");
    }
    else if (token->operators == PLUS)
    {
        printf("+");
    }
    else if (token->operators == MINUS)
    {
        printf("-");
    }
    else if (token->operators == CONCAT)
    {
        printf(".");
    }
    else if (token->operators == LESS)
    {
        printf("<");
    }
    else if (token->operators == GREATER)
    {
        printf(">");
    }
    else if (token->operators == LESS_EQUAL)
    {
        printf("<=");
    }
    else if (token->operators == GREATER_EQUAL)
    {
        printf(">=");
    }
    else if (token->operators == EQUALS)
    {
        printf("=");
    }
    else if (token->operators == NOT_EQUAL)
    {
        printf("!=");
    }
    else if (token->operators == TYPE_EQUALS)
    {
        printf("===");
    }
    else if (token->operators == TYPE_NOT_EQUALS)
    {
        printf("!==");
    }
}

enum T_KEYWORD get_keyword_from_token(TOKEN_T* token)
{
    if (token->value.type == 0)
    {
        return KEY_INT;
    }
    if (token->value.type == 1)
    {
        return KEY_STRING;
    }
    if (token->value.type == 2)
    {
        return KEY_FLOAT;
    }

    return NULL_KEYWORD;
}