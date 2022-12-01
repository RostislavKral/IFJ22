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
    ST_LEFT_PARENTHESES,
    ST_RIGHT_PARENTHESES,
    ST_LEFT_CURLYBRACKET,
    ST_RIGHT_CURLYBRACKET,
    ST_DATA_TYPE,
    ST_COMMA,
    ST_KEYWORD_FLOAT,
};

TOKEN_T * get_next_token();

#endif