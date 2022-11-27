//
// Created by rostik on 10/20/22.
//


#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"


enum T_STATE {
    ST_START,
    ST_VAR_PREFIX,
    ST_VAR,
    ST_OP_ASSIGN,
    ST_FUNC,
    ST_ERROR,
    ST_INT_LITERAL,
    ST_DOUBLE_LITERAL,
    ST_STRING_LITERAL,
};

TOKEN_T * get_next_token(FILE* file);

#endif