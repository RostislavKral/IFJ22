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


bool is_keyword(char* keyword)
{
    return 0;
}

bool is_data_type(char* data_type)
{
    //if(strcmp())
}

/*
struct T_TOKEN make_token()
{

}
*/







TOKEN_T * get_next_token()
{
    TOKEN_T * token;
    token = malloc(sizeof(TOKEN_T));

    enum T_STATE state = ST_START;

    char *edge = malloc(sizeof (char));
    char tmp;

    DYN_STRING_T str;
    str_init(&str);

    while(true){

        tmp = getc(stdin);
       // printf("%c\n", tmp);
        *edge = tmp;
        if(*edge == EOF){
            token->type = ISEOF;
            token->name = NULL;
            return token;
        }

        switch (state)
        {
            case ST_START:
                if(*edge  == '(') state = ST_LEFT_PARENTHESES;
                if(*edge  == ')') state = ST_RIGHT_PARENTHESES;
                if(*edge  == '{') state = ST_RIGHT_CURLYBRACKET;
                if(*edge  == '}') state = ST_LEFT_CURLYBRACKET;
                if(*edge  == ',') state = ST_COMMA;

                if (*edge == '$') {
                    state = ST_VAR_PREFIX;
                    token->type = TOKEN_ID;
                }

                if(*edge == '=') state = ST_OP_ASSIGN;
                if(*edge == 'f') {
                    int isFunction = 1;
                    int isFloat = 1;
                    char c;
                    char pref[] = { 'n', 'c', 't', 'i', 'o', 'n'};
                    char floatType[] = {'o', 'a', 't'};
                    c = fgetc(stdin);
                    if (c == 'u') {

                        for (int i = 0; i < 6; i++) {
                            c = fgetc(stdin);
                            if (c != pref[i]) isFunction = 0;
                            if (!isFunction) {
                                exit(1);
                                /*state = ERROR;
                                token->type = ERROR;
                                token->name = NULL;
                                break; */
                            }
                        }

                        state = ST_FUNC;

                        printf("\n\nIs function: %d\n\n", isFunction);
                    } else if(c == 'l') {
                        for (int i = 0; i < 3; i++) {
                            c = fgetc(stdin);
                            if (c != floatType[i]) isFloat = 0;
                            if (!isFloat) {
                                exit(1);
                                /*state = ERROR;
                                token->type = ERROR;
                                token->name = NULL;
                                break; */
                            }
                        }

                       // state = ST_KEYWORD;
                        //printf("float je to ");
                    } else {
                        exit(1);
                    }

                    if(fgetc(stdin) != ' '  ) exit(1);

                }

                if(*edge == 'w') {
                    char c;
                    char pref[] = { 'h', 'i', 'l', 'e'};
                    bool isValid = 1;

                    for (int i = 0; i < 3; i++) {
                        c = fgetc(stdin);
                        if (c != pref[i]) isValid = 0;
                        if (!isValid) {
                            exit(1);
                            /*state = ERROR;
                            token->type = ERROR;
                            token->name = NULL;
                            break;*/
                        }
                    }

                    if(fgetc(stdin) != ' '  ) exit(1);
                    //state = ST_KEYWORD;
                }

                /* This was added by SniehNikita */
                if(isdigit(*edge)) {
                    state = ST_INT_LITERAL;
                    ungetc(*edge, stdin);
                }
                /* ----------------------------- */


            case ST_VAR_PREFIX:
                if(isalnum(*edge) && state ==  ST_VAR_PREFIX) {
                    str_conc(&str, edge);
                    state = ST_VAR;
                }
                break;
            case ST_VAR:

                if(*edge != ' ' && isalnum(*edge)){
                    str_conc(&str, edge);
                     continue;
                }
                ungetc(*edge, stdin);
                token->type = TOKEN_ID;
                token->name = str.str;

                return token;
            case ST_OP_ASSIGN:
                ungetc(*edge, stdin);

                token->type = ASSIGN;
                token->name = NULL;
                return token;
            case ST_FUNC:

                if(*edge != ' ' && isalnum(*edge)){
                    str_conc(&str, edge);
                    continue;
                }
                ungetc(*edge, stdin);

                token->type = FUNC_ID;
                token->name = str.str;
                return token;

            case ST_ERROR:
                break;
            
            /* This was added by SniehNikita */
            case ST_INT_LITERAL:
                if (isdigit(*edge)) {
                    // TODO might be overflow if too big number
                    token->value.int_val = token->value.int_val * 10 + (*edge - '0');
                } /* else if (*edge == '.') {
                    // TODO float state
                } */ else {
                    ungetc(*edge, stdin);
                    token->type = LITERAL;
                    token->value.type = 0;
                    return token;
                }
                break;
            /* ----------------------------- */
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
        }



    }

}
/*
switch (state) {
    case ID: str += a;
    iskeyword(str);

}
*/