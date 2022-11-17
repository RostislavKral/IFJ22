//
// Created by rostik on 10/20/22.
//

#ifndef IFJ2_LEXER_H
#define IFJ2_LEXER_H
#include <stdio.h>
#endif //IFJ2_LEXER_H


enum T_TOKEN_TYPE
{
    KEYWORD,
    TOKEN_ID,
    FUNC_ID,
    STR_LIT,
    NUM_LIT,
    ASSIGN,
    LPAR,
    RPAR,
    OPERATOR,
    ISEOF,
    //TBD
};


enum T_STATE {
    START,
    VAR_PREFIX,
    VAR,
    OP_ASSIGN,
    FUNC,
    ERROR,
    LITERAL,
};


enum T_KEYWORD {
    function,
    while_loop,
    do_while_loop,
    predialog,

};


struct T_LITERAL
{
    int type; // 0 - int, 1 - char *, 2 - double
    float float_val;
    int int_val;
    char *char_val;
};



struct T_TOKEN {

    enum T_TOKEN_TYPE type;
    char* name;
    struct T_LITERAL value;

};
struct T_TOKEN get_next_token(FILE* file);