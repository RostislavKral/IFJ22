//
// Created by rostik on 10/20/22.
//

#include "lexer.h"


#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utilities/dyn_string.h"

#define bool int
#define true 1
#define false 0


bool is_keyword(char* keyword)
{
    return 0;
}

/*
struct T_TOKEN make_token()
{

}
*/







struct T_TOKEN get_next_token(FILE *file)
{
    struct T_TOKEN token;
    enum T_STATE state = START;

    char *edge = malloc(sizeof (char));
    char tmp;
    dyn_string str;
    str.inc = 1;
    str.max_len = 1;

    str.str = malloc (str.max_len * sizeof(char));


    while(true){

        tmp = getc(file);
        *edge = tmp;
        if(*edge == EOF){
            token.type = ISEOF;
            token.name = NULL;
            return token;
        }

        switch (state)
        {
            case START:
                if (*edge == '$') {
                    state = VAR_PREFIX;
                    token.type = TOKEN_ID;
                }

                if(*edge == '=') state = OP_ASSIGN;
                if(*edge == 'f') {
                    int isFunction = 1;
                    char c;
                    char pref[] = {'u', 'n', 'c', 't', 'i', 'o', 'n'};
                    for (int i = 0; i < 7; i++) {
                        c = fgetc(file);
                        if(c != pref[i]) isFunction = 0;
                        if(!isFunction) {
                            exit(1);
                            /*state = ERROR;
                            token.type = ERROR;
                            token.name = NULL;
                            break; */
                        }


                    }

                    if(fgetc(file) != ' '  ) exit(1);
                    state = FUNC;

                    printf("\n\nIs function: %d\n\n", isFunction);
                }


            case VAR_PREFIX:
                if(isalnum(*edge) && state ==  VAR_PREFIX) {
                    str_conc(&str, edge);
                    state = VAR;
                }
                break;
            case VAR:

                if(*edge != ' ' && isalnum(*edge)){
                    str_conc(&str, edge);
                     continue;
                }
                token.type = TOKEN_ID;
                token.name = str.str;

                return token;
            case OP_ASSIGN:
                token.type = ASSIGN;
                token.name = NULL;
                return token;
            case FUNC:
                token.type = FUNC_ID;
                token.name = NULL;
                return token;

            case ERROR:
                break;
            case LITERAL:
                break;
        }



    }

}
/*
switch (state) {
    case ID: str += a;
    iskeyword(str);

}
*/