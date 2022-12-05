//
// Created by rostik on 10/20/22.
//

#include "lexer.h"
#include "token.h"
#include "dyn_string.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


#define bool int
#define true 1
#define false 0


int is_keyword(char *keyword) {
    char *keywords[] = {"else", "float", "function", "if", "int", "null", "return", "string", "while", "void", "colon"};

    for (int i = 0; i < 11; i++) {
        if (strcmp(keyword, keywords[i]) == 0) {
            return i;
        }
    }

    return -1;
}

bool is_program_start(char* keyword)
{
    return strcmp(keyword, "<?php") == 0;
}

bool is_program_end(char* keyword)
{
    return strcmp(keyword, "?>") == 0;
}


enum T_STATE getFunctionCallOrKeywordLexeme(char *keyword) {
    int lexeme = is_keyword(keyword);
    if (lexeme >= 0) {
        switch (lexeme) {
            case 0:
                return ST_KEYWORD_ELSE;
            case 1:
                return ST_KEYWORD_FLOAT;
            case 2:
                return ST_FUNC;
            case 3:
                return ST_KEYWORD_IF;
            case 4:
                return ST_KEYWORD_INT;
            case 5:
                return ST_KEYWORD_NULL;
            case 6:
                return ST_KEYWORD_RETURN;
            case 7:
                return ST_KEYWORD_STRING;
            case 8:
                return ST_KEYWORD_WHILE;
            case 9:
                return ST_KEYWORD_VOID;
            case 10:
                return ST_KEYWORD_COLON;


        }
    }
    else if(is_program_start(keyword))
    {
        return ST_PROG_START;
    }
    else if(is_program_end(keyword))
    {
        return ST_PROG_END;
    }
    else {
        return ST_FUNC_CALL;
    }

}

bool is_data_type(char *data_type) {
    //if(strcmp())
}

/*
struct T_TOKEN make_token()
{

}
*/




bool control = 0;
char *c;
int line_number = 1;
int char_number = 0;

void lexer_unget(char edge)
{
    char_number--;
    if (edge == '\n')
    {
        line_number--;
    }

    ungetc(edge, stdin);
}

char lexer_fget()
{
    char edge = fgetc(stdin);
    char_number++;
    if (edge == '\n')
    {
        line_number++;
        char_number = 0;
    }

    return edge;
}

void set_line_num(TOKEN_T* token)
{
    token->lineNum = line_number;
    token->charNum = char_number;
}

TOKEN_T *get_next_token() {
    TOKEN_T *token;
    token = malloc(sizeof(TOKEN_T));
    token->value.int_val = 0;
    enum T_STATE state = ST_START;

    char *edge = malloc(sizeof(char));
    bool read = 1;

    DYN_STRING_T str, fun_name;
    str_init(&str);
    str_init(&fun_name);


    while (true) {
//        *edge = fgetc(stdin);
        *edge = lexer_fget();
        edge[1] = '\0';
//        printf("__________%c", *edge);
//        printf("%d", state);

        // str_conc(&str, edge);
/*
       if(control == 0 && state == ST_READ)
       {
           control = 1;
           str_conc(&str, edge);
           printf("%s \n\n\n", str.str);
       }*/



   /*     char eof_tmp = lexer_fget();
        printf("%c", eof_tmp);
        if(eof_tmp==EOF){
            token->type = ISEOF;
            token->name = NULL;

            set_line_num(token);
            return token;
        } else {
            lexer_unget(eof_tmp);
        }*/
        switch (state) {
            case ST_START:
                if (*edge == EOF) {
                    token->type = ISEOF;
                    token->name = NULL;

                    set_line_num(token);
                    return token;
                }
                else if (*edge == '(') state = ST_LEFT_PARENTHESES;
                else if (*edge == ' ') state = ST_START;
                else if (*edge == '\n') state = ST_START;
                else if (*edge == ')') state = ST_RIGHT_PARENTHESES;
                else if (*edge == '{') state = ST_LEFT_CURLYBRACKET;
                else if (*edge == '}') state = ST_RIGHT_CURLYBRACKET;
                else if (*edge == ',') state = ST_COMMA;
                else if (*edge == ':') state = ST_KEYWORD_COLON;
                else if (*edge == '-') state = ST_OP_MINUS;
                else if (*edge == '+') state = ST_OP_PLUS;
                else if (*edge == '/') state = ST_OP_DIVIDE;
                else if (*edge == '*') state = ST_OP_MULTIPLY;
                else if (*edge == '.') state = ST_OP_CONCAT;
                else if (*edge == '<')
                {
                    char tmp = lexer_fget();
                    if (tmp == '?')
                    {
                        lexer_unget(tmp);
                        lexer_unget(*edge);
                        state = ST_READ;
                    }
                    else
                    {
                        lexer_unget(tmp);
                        state = ST_OP_LESSER_THAN;
                    }
                }
                else if (*edge == '>') state = ST_OP_GREATER_THAN;
                else if (*edge == '"') state = ST_STRING_LITERAL;
                else if (*edge == ';') state = ST_SEMICOLON;
                else if (*edge == '!') state = ST_OP_NOT_EQUAL;
                    //else if (*edge == ' ' || *edge == '\n') state = ST_START;
                    //TODO: OPERATORY!!!!!!


                else if (*edge == '$') {
                    state = ST_VAR_PREFIX;
                    token->type = TOKEN_ID;
                } else if (*edge == '=') state = ST_OP_ASSIGN;
                else {
                    //printf("____%s", edge);
                    state = ST_READ;
                    lexer_unget(*edge);
                }


                /* This was added by SniehNikita */
                if (isdigit(*edge)) {
                    state = ST_INT_LITERAL;
                    lexer_unget(*edge);
                }
                /* ----------------------------- */


            case ST_VAR_PREFIX:
                if (isalnum(*edge) && state == ST_VAR_PREFIX) {
                    str_conc(&str, edge);
                    state = ST_VAR;
                }

                break;
            case ST_SLINE_COMMENT:
                if (*edge == '\n') state = ST_START;
                break;
            case ST_PROG_START:
                token->type = PROG_START;

                set_line_num(token);
                return token;
            case ST_PROG_END:
                token->type = PROG_END;

                set_line_num(token);
                return token;
            case ST_OP_TYPE_NOT_EQUALS:
                token->type = OPERATOR;
                token->operators = TYPE_NOT_EQUALS;
                token->name = NULL;

                set_line_num(token);
                return token;
            case ST_OP_NOT_EQUAL:
                if (*edge != '=') exit(1);
                char tmp = *edge;
                *edge = lexer_fget();
                if (tmp == '=' && *edge == '=') {
                    state = ST_OP_TYPE_NOT_EQUALS;
                    break;

                }
                if (tmp == '=' && (*edge == ' ' || *edge == '\n')) {
                    token->type = OPERATOR;
                    token->operators = NOT_EQUAL;
                    token->name = NULL;

                    set_line_num(token);
                    return token;
                }
                exit(1);


            case ST_MLINE_COMMENT:

                if (*edge == '/') state = ST_START;
                break;
            case ST_VAR:

                if (*edge != ' ' && isalnum(*edge)) {
                    str_conc(&str, edge);
                    continue;
                }
                lexer_unget(*edge);
                token->type = TOKEN_ID;
                token->name = str.str;

                set_line_num(token);
                return token;

            case ST_OP_TYPE_EQUALS:
                token->type = OPERATOR;
                token->operators = TYPE_EQUALS;
                token->name = NULL;

                lexer_unget(*edge);
                set_line_num(token);
                return token;
            case ST_OP_EQUALS:
                if (*edge == '=') {
                    state = ST_OP_TYPE_EQUALS;
                    break;
                }
                token->type = OPERATOR;
                token->operators = EQUALS;
                token->name = NULL;

                lexer_unget(*edge);
                set_line_num(token);
                return token;
            case ST_OP_ASSIGN:
                if (*edge == '=') {
                    state = ST_OP_EQUALS;
                    break;
                }
                lexer_unget(*edge);

                token->type = ASSIGN;
                token->name = NULL;

                set_line_num(token);
                return token;
            case ST_OP_PLUS:
                lexer_unget(*edge);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = PLUS;

                set_line_num(token);
                return token;
            case ST_OP_MINUS:
                lexer_unget(*edge);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = MINUS;

                set_line_num(token);
                return token;
            case ST_OP_MULTIPLY:
                lexer_unget(*edge);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = MULTIPLY;

                set_line_num(token);
                return token;
            case ST_OP_DIVIDE:

                if (*edge != '/' && *edge != '*') {
                    lexer_unget(*edge);

                    token->type = OPERATOR;
                    token->name = NULL;
                    token->operators = DIVIDE;

                    set_line_num(token);
                    return token;
                } else if (*edge == '*') {
                    state = ST_MLINE_COMMENT;
                    break;
                } else if (*edge == '/') {
                    state = ST_SLINE_COMMENT;
                    break;
                }
            case ST_OP_CONCAT:
                lexer_unget(*edge);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = CONCAT;

                set_line_num(token);
                return token;
            case ST_OP_LESSER_THAN:
                if (*edge == '=') {
                    state = ST_OP_LESS_EQUAL;
                    break;
                }
                lexer_unget(*edge);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = LESS;

                set_line_num(token);
                return token;
            case ST_OP_LESS_EQUAL:
                lexer_unget(*edge);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = LESS_EQUAL;

                set_line_num(token);
                return token;
            case ST_OP_GREATER_EQUAL:
                lexer_unget(*edge);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = GREATER_EQUAL;

                set_line_num(token);
                return token;
            case ST_OP_GREATER_THAN:
                if (*edge == '=') {
                    state = ST_OP_GREATER_EQUAL;
                    break;
                }
                lexer_unget(*edge);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = GREATER;

                set_line_num(token);
                return token;
            case ST_FUNC:
                //printf("%c", *edge);
                /*if (*//**edge != ' ' &&*//* isalnum(*edge)) {
                    str_conc(&fun_name, edge);
                    continue;
                }*
                lexer_unget(*edge);


*/
                if (*edge == ' ' || *edge == '(' || *edge == '\n') {

                    //  exit(1);
                    if (*edge == '(') {
                        lexer_unget(*edge);
                    }

                    token->type = FUNC_ID;
                    token->name = fun_name.str;

                    set_line_num(token);
                    return token;

                    //str_destroy(&str);
                    // printf("________%s", str.str);
                    // printf("%d",state);

                }
                    // printf("%c", *edge);
                else {
                    state = ST_FUNC;
                    str_conc(&fun_name, edge);

                }

            case ST_ERROR:
                break;

                /* This was added by SniehNikita */
            case ST_INT_LITERAL:
                //token->value.int_val = 0;
                token->value.double_val = 0;
                int isValid = 0;

                //printf("%c\n", *edge);
                if (isdigit(*edge)) {
                    // TODO might be overflow if too big number
                    if(read != 1) {
                        token->value.int_val = token->value.int_val * 10 + (*edge - '0');
                    }

                    read = 0;
                } else if (*edge == '.') {
                    // char c;
                    double test, tmp;
                    int offset = 10;
                    *edge = lexer_fget();

                    while (isdigit(*edge)) {
                        tmp = (double) *edge - '0';
                        test += tmp / offset;
                        *edge = lexer_fget();

                        offset *= 10;
                    }

                    token->value.double_val = test;
                    isValid = 1;
                    lexer_unget(*edge);
                    token->type = LITERAL;
                    token->value.type = 2;
                    token->value.double_val += token->value.int_val;
                    token->value.int_val = 0;

                    set_line_num(token);
                    return token;
                } else {
                    //printf("%d", isValid);

                    lexer_unget(*edge);
                    token->type = LITERAL;
                    if (isValid == 1) {
                        token->value.type = 2;
                        token->value.double_val += token->value.int_val;
                        token->value.int_val = 0;
                    } else {
                        token->value.type = 0;
                    }

                    set_line_num(token);
                    return token;
                }


                //exit(1);
                break;
                /* ----------------------------- */

            case ST_STRING_LITERAL:

                if (*edge != '"') {

                    str_conc(&str, edge);
                    // *edge = lexer_fget();
                    // if (*edge == '"') break;
                } else {

                   // lexer_unget(*edge);
                    token->type = LITERAL;
                    token->value.char_val = str.str;
                    token->value.type = 1;

                    set_line_num(token);
                    return token;
                }
                break;
            case ST_LEFT_PARENTHESES:
                token->type = LPAR;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_RIGHT_PARENTHESES:
                token->type = RPAR;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_LEFT_CURLYBRACKET:
                token->type = LBRACE;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_RIGHT_CURLYBRACKET:
                token->type = RBRACE;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_COMMA:
                token->type = COMMA;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_FLOAT:
                token->type = KEYWORD;
                token->keyword = KEY_FLOAT;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_INT:
                token->type = KEYWORD;
                token->keyword = KEY_INT;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_STRING:
                token->type = KEYWORD;
                token->keyword = KEY_STRING;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_COLON:
                token->type = KEYWORD;
                token->keyword = KEY_COLON;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_IF:
                token->type = KEYWORD;
                token->keyword = KEY_IF;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_VOID:
                token->type = KEYWORD;
                token->keyword = KEY_VOID;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_RETURN:
                token->type = KEYWORD;
                token->keyword = KEY_RETURN;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_ELSE:
                token->type = KEYWORD;
                token->keyword = KEY_ELSE;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_WHILE:
                token->type = KEYWORD;
                token->keyword = KEY_WHILE_LOOP;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_KEYWORD_NULL:
                token->type = KEYWORD;
                token->keyword = KEY_NULL;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_SEMICOLON:
                token->type = SEMICOLON;
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_FUNC_CALL:
                token->type = FUNC_CALL;
                token->name = str.str;
                // printf("__________%s", str.str);
                //str_destroy(&str);
                lexer_unget(*edge);

                set_line_num(token);
                return token;
            case ST_READ:
//                printf(" ");
                // printf("______%c", *edge);

                if (*edge == ' ' || *edge == '(' || *edge == '\n' || *edge == ')' || *edge == '{' || *edge == '}' || *edge == ';') {

                    //  exit(1);
                    if (*edge != ' ')
                    {
                        lexer_unget(*edge);
                    }

                    state = getFunctionCallOrKeywordLexeme(str.str);

                    //str_destroy(&str);
                    // printf("________%s", str.str);
                    // printf("%d",state);
                    break;
                }
                    // printf("%c", *edge);
                else {
                    state = ST_READ;
                    //printf("%c-%d-%d ", *edge, line_number, char_number);
//                    printf("%s", str.str);
                    str_conc(&str, edge);

                }
                // printf("____string: %s\n", str.str);
                //   break;
                //   exit(1);
                /* default:
                     printf(" ");
                     //state = ST_READ;*/
        }


    }

}