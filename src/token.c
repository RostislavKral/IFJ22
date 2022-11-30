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