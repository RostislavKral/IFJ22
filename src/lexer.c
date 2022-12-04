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
    } else {
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

TOKEN_T *get_next_token() {
    TOKEN_T *token;
    token = malloc(sizeof(TOKEN_T));

    enum T_STATE state = ST_START;

    char *edge = malloc(sizeof(char));


    DYN_STRING_T str, fun_name;
    str_init(&str);
    str_init(&fun_name);


    while (true) {
        *edge = fgetc(stdin);
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

        if (*edge == EOF) {
            token->type = ISEOF;
            token->name = NULL;
            return token;
        }

        switch (state) {
            case ST_START:

                if (*edge == '(') state = ST_LEFT_PARENTHESES;
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
                else if (*edge == '<') state = ST_OP_LESSER_THAN;
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
                    ungetc(*edge, stdin);
                }


                /* This was added by SniehNikita */
                if (isdigit(*edge)) {
                    state = ST_INT_LITERAL;
                    ungetc(*edge, stdin);
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

            case ST_OP_TYPE_NOT_EQUALS:
                token->type = OPERATOR;
                token->operators = TYPE_NOT_EQUALS;
                token->name = NULL;
                return token;
            case ST_OP_NOT_EQUAL:
                if (*edge != '=') exit(1);
                char tmp = *edge;
                *edge = fgetc(stdin);
                if (tmp == '=' && *edge == '=') {
                    state = ST_OP_TYPE_NOT_EQUALS;
                    break;

                }
                if (tmp == '=' && (*edge == ' ' || *edge == '\n')) {
                    token->type = OPERATOR;
                    token->operators = NOT_EQUAL;
                    token->name = NULL;
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
                ungetc(*edge, stdin);
                token->type = TOKEN_ID;
                token->name = str.str;

                return token;

            case ST_OP_TYPE_EQUALS:
                token->type = OPERATOR;
                token->operators = TYPE_EQUALS;
                token->name = NULL;
                return token;
            case ST_OP_EQUALS:
                if (*edge == '=') {
                    state = ST_OP_TYPE_EQUALS;
                    break;
                }
                token->type = OPERATOR;
                token->operators = EQUALS;
                token->name = NULL;
                return token;
            case ST_OP_ASSIGN:
                if (*edge == '=') {
                    state = ST_OP_EQUALS;
                    break;
                }
                ungetc(*edge, stdin);

                token->type = ASSIGN;
                token->name = NULL;
                return token;
            case ST_OP_PLUS:
                ungetc(*edge, stdin);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = PLUS;
                return token;
            case ST_OP_MINUS:
                ungetc(*edge, stdin);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = MINUS;
                return token;
            case ST_OP_MULTIPLY:
                ungetc(*edge, stdin);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = MULTIPLY;
                return token;
            case ST_OP_DIVIDE:

                if (*edge != '/' && *edge != '*') {
                    ungetc(*edge, stdin);

                    token->type = OPERATOR;
                    token->name = NULL;
                    token->operators = DIVIDE;
                    return token;
                } else if (*edge == '*') {
                    state = ST_MLINE_COMMENT;
                    break;
                } else if (*edge == '/') {
                    state = ST_SLINE_COMMENT;
                    break;
                }
            case ST_OP_CONCAT:
                ungetc(*edge, stdin);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = CONCAT;
                return token;
            case ST_OP_LESSER_THAN:
                if (*edge == '=') {
                    state = ST_OP_LESS_EQUAL;
                    break;
                }
                ungetc(*edge, stdin);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = LESS;
                return token;
            case ST_OP_LESS_EQUAL:
                ungetc(*edge, stdin);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = LESS_EQUAL;
                return token;
            case ST_OP_GREATER_EQUAL:
                ungetc(*edge, stdin);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = GREATER_EQUAL;
                return token;
            case ST_OP_GREATER_THAN:
                if (*edge == '=') {
                    state = ST_OP_GREATER_EQUAL;
                    break;
                }
                ungetc(*edge, stdin);

                token->type = OPERATOR;
                token->name = NULL;
                token->operators = GREATER;
                return token;
            case ST_FUNC:
                //printf("%c", *edge);
                /*if (*//**edge != ' ' &&*//* isalnum(*edge)) {
                    str_conc(&fun_name, edge);
                    continue;
                }*
                ungetc(*edge, stdin);


*/
                if (*edge == ' ' || *edge == '(' || *edge == '\n') {

                    //  exit(1);
                    if (*edge == '(') {
                        ungetc(*edge, stdin);
                    }

                    token->type = FUNC_ID;
                    token->name = fun_name.str;
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
                // token->value.int_val = 0;
                token->value.double_val = 0;
                int isValid = 0;


                if (isdigit(*edge)) {
                    // TODO might be overflow if too big number
                    token->value.int_val = token->value.int_val * 10 + (*edge - '0');
                } else if (*edge == '.') {
                    // char c;
                    double test, tmp;
                    int offset = 10;
                    *edge = fgetc(stdin);

                    while (isdigit(*edge)) {
                        tmp = (double) *edge - '0';
                        test += tmp / offset;
                        *edge = fgetc(stdin);

                        offset *= 10;
                    }

                    token->value.double_val = test;
                    isValid = 1;
                    ungetc(*edge, stdin);
                    token->type = LITERAL;
                    token->value.type = 2;
                    token->value.double_val += token->value.int_val;
                    token->value.int_val = 0;
                    return token;
                } else {
                    //printf("%d", isValid);

                    ungetc(*edge, stdin);
                    token->type = LITERAL;
                    if (isValid == 1) {
                        token->value.type = 2;
                        token->value.double_val += token->value.int_val;
                        token->value.int_val = 0;
                    } else {
                        token->value.type = 0;
                    }
                    return token;
                }


                //exit(1);
                break;
                /* ----------------------------- */

            case ST_STRING_LITERAL:

                if (*edge != '"') {

                    str_conc(&str, edge);
                    // *edge = fgetc(stdin);
                    // if (*edge == '"') break;
                } else {

                    ungetc(*edge, stdin);
                    token->type = LITERAL;
                    token->value.char_val = str.str;
                    token->value.type = 1;
                    return token;
                }
                break;
            case ST_LEFT_PARENTHESES:
                token->type = LPAR;
                ungetc(*edge, stdin);

                return token;
            case ST_RIGHT_PARENTHESES:
                token->type = RPAR;
                ungetc(*edge, stdin);
                return token;
            case ST_LEFT_CURLYBRACKET:
                token->type = LBRACE;
                ungetc(*edge, stdin);
                return token;
            case ST_RIGHT_CURLYBRACKET:
                token->type = RBRACE;
                ungetc(*edge, stdin);
                return token;
            case ST_COMMA:
                token->type = COMMA;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_FLOAT:
                token->type = KEYWORD;
                token->keyword = KEY_FLOAT;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_INT:
                token->type = KEYWORD;
                token->keyword = KEY_INT;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_STRING:
                token->type = KEYWORD;
                token->keyword = KEY_STRING;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_COLON:
                token->type = KEYWORD;
                token->keyword = KEY_COLON;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_IF:
                token->type = KEYWORD;
                token->keyword = KEY_IF;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_VOID:
                token->type = KEYWORD;
                token->keyword = KEY_VOID;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_RETURN:
                token->type = KEYWORD;
                token->keyword = KEY_RETURN;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_ELSE:
                token->type = KEYWORD;
                token->keyword = KEY_ELSE;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_WHILE:
                token->type = KEYWORD;
                token->keyword = KEY_WHILE_LOOP;
                ungetc(*edge, stdin);
                return token;
            case ST_KEYWORD_NULL:
                token->type = KEYWORD;
                token->keyword = KEY_NULL;
                ungetc(*edge, stdin);
                return token;
            case ST_SEMICOLON:
                token->type = SEMICOLON;
                ungetc(*edge, stdin);
                return token;
            case ST_FUNC_CALL:
                token->type = FUNC_CALL;
                token->name = str.str;
                // printf("__________%s", str.str);
                //str_destroy(&str);
                ungetc(*edge, stdin);
                return token;
            case ST_READ:
//                printf(" ");
                // printf("______%c", *edge);

                if (*edge == ' ' || *edge == '(' || *edge == '\n') {

                    //  exit(1);
                    if (*edge == '(') {
                        ungetc(*edge, stdin);
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