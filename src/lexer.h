//
// Created by rostik on 10/20/22.
//


#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"

//LEXEMY
enum T_STATE {
    ST_START,               // 1
    ST_VAR_PREFIX,          // 2
    ST_VAR,                 // 3
    ST_OP_ASSIGN,
    ST_OP_PLUS,
    ST_OP_MINUS,
    ST_OP_DIVIDE,
    ST_OP_MULTIPLY,
    ST_OP_CONCAT,
    ST_OP_LESSER_THAN,      // 10
    ST_OP_GREATER_THAN,
    ST_OP_LESS_EQUAL,         //7  <=
    ST_OP_GREATER_EQUAL,      //8  >=
    ST_OP_EQUALS,             //9  ==
    ST_OP_NOT_EQUAL,          //10 !=
    ST_OP_TYPE_EQUALS,        //11 ===
    ST_OP_TYPE_NOT_EQUALS,    //12 !==
    //TBD
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
    ST_KEYWORD_STRING,
    ST_KEYWORD_INT,
    ST_KEYWORD_COLON,
    ST_KEYWORD_ELSE,
    ST_KEYWORD_IF,
    ST_KEYWORD_WHILE,
    ST_KEYWORD_RETURN,
    ST_KEYWORD_NULL,
    ST_KEYWORD_VOID,
    ST_FUNC_CALL,
    ST_SEMICOLON,
    ST_READ,
    ST_SLINE_COMMENT,
    ST_MLINE_COMMENT,
    ST_PROG_START,
    ST_PROG_END
};

TOKEN_T * get_next_token();
void lexer_unget(char edge);
char lexer_fget();
void set_line_num(TOKEN_T* token);

#endif