/**
 * @file syntax_and_semantic_analyzer.c
 * @author Nikita Sniehovskyi xsnieh00
 * @author Lukáš Ježek <xjezek19@stud.fit.vutbr.cz>
 * @brief syntax and semantiv analyzer implementation
 * @version 0.1
 * @date 2022-11-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "syntax_and_semantic_analyzer.h"
#include "lexer.h"

bool verify_eof(TOKEN_T token){
    if(token.type == ISEOF){
        fprintf(stderr, "%s", "EOF");
        return true;
    } else{
        return false;
    }
}

void function_detected(TOKEN_T token){
    TOKEN_T *nextToken = get_next_token();
    if(!verify_eof(*nextToken)){
        if(nextToken->type == LPAR)
    }
};

void analyze_token_key(TOKEN_T token){
    switch (token.keyword) {
        case KEY_ELSE:
            break;
        case KEY_FLOAT:
            break;
        case KEY_FUNCTION:
            function_detected(token);
            break;
        case KEY_IF:
            break;
        case KEY_INT:
            break;
        case KEY_NULL:
            break;
        case KEY_RETURN:
            break;
        case KEY_STRING:
            break;
        case KEY_WHILE_LOOP:
            break;
        case KEY_VOID:
            break;
    }
};


int SNS_init() {


    return 0;
}