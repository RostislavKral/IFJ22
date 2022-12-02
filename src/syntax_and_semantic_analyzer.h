/**
 * @file syntax_and_semantic_analyzer.h
 * @author Nikita Sniehovskyi xsnieh00
 * @author Lukáš Ježek <xjezek19@stud.fit.vutbr.cz>
 * @brief 
 * @version 0.1
 * @date 2022-11-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SYNTAX_AND_SEMANTIC_ANALYZER_H
#define SYNTAX_AND_SEMANTIC_ANALYZER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "symtable.h"
#include "lexer.h"
#include "token.h"
//#include "codegen.h"


/*

rule list: list of program

<?php
$a = 5

in rule list will be:

rl = [
    rule: start
    rule: assign
]

rl is:
    [ start of the program ]
    [ function declaration ]
    [ main program ]
    [ end of the program ]
    [ pointer to the each section ]


main program start: <?php, end: EOF
function start: {, end: }


rule:
    assign
    if



*/


enum RULE_TYPE_T {
                            //     T_TOKEN_TYPE or RULE_TYPE_T

    T_START_OF_PROGRAM,     // 0 - PROG_START
    T_END_OF_PROGRAM,       // 1 - ISEOF
    T_ASSIGN_VALUE,         // 2 - TOKEN_ID ASSIGN [expression] SEMICOLON
    T_EXPRESSION,           // 3 - [value] OPERATOR [value]
    T_VALUE,                // 4 - TOKEN_ID or LITERAL

    T_UNDEFINED,
};

/* ------ Rules description ------ */
//wtf block
typedef struct {
    TOKEN_T * value_token;
} R_VALUE;

typedef struct EXPRESSION_T {
    // TODO What if expression contains function return value?

    R_VALUE * value_left_token;
    struct EXPRESSION_T * expr_left_token;

    TOKEN_T * operator_token;

    R_VALUE * value_right_token;
    struct EXPRESSION_T * expr_right_token;
} R_EXPRESSION;

typedef struct {
    TOKEN_T * token_id_token;
    TOKEN_T * assign_token;
    R_EXPRESSION * expression;
    TOKEN_T * semicolon;
} R_ASSIGN_VALUE;
// wtf block end
typedef struct {
    TOKEN_T * eof_token;
} R_END_OF_PROGRAM;

typedef struct {
    TOKEN_T * start_token;
} R_START_OF_PROGRAM;

/* --- End of rules description --- */



/* ----- Single rule structure ----- */

/**
 * @brief Rule type list
 * 
 */
typedef union {
    R_START_OF_PROGRAM rule_sop;
    R_END_OF_PROGRAM rule_eop;
    R_ASSIGN_VALUE rule_av;
    R_EXPRESSION rule_e;
    R_VALUE rule_v;
} RULE_CONTENT_T;

/**
 * @brief Rule structure
 * 
 */
typedef struct {
    enum RULE_TYPE_T type;
    RULE_CONTENT_T rule;
} RULE_T;

/* - End of single rule structure - */

enum SNS_ERROR {
    OK,                 // 0
    UNKNOWN_COMMAND,    // 1
    UNEXPECTED_TOKEN,   // 2
};

//jeza

////Linked list for local variables
//typedef struct frameElement{
//    TOKEN_T localVariableToken;
//    struct frameElement *nextElement;
//} *frameElementPtr;
//
//typedef struct List{
//    frameElementPtr firstElement;
//} frameList;

typedef struct {
    bool fParsing;
    bool fHeadParsed;
    bool fNamePass;
    bool fParamPass;
    bool fBodyParsing;
    bool fReturnTypePass;
    int fParamCount;
    int fBraceCountCheck;
} parseFunctionHelper;

typedef struct {
    //scope number
    int num;
    //opened braces eg. fun(){ if(){ == 2 opened braces
    int openedBracesCount;
    TOKEN_T *lastScopeOpeningToken;
    //for function definitions
    htab_t* globalSymTable;
    //for local vars frames
    htab_t* localSymTable;
} scopeHelper;

void analyze_token();
void function_detected(TOKEN_T* token);
void function_end_parsing();
void parse_expression();

////LL functions
//frameList *create_new_frame();
//void remove_frame(frameList *list);
////insert local var to frame, always to first position
//void insert_to_frame(frameList *frame, TOKEN_T token);
////search for token if not found return NULL
//frameElementPtr search_in_frame(frameList frame, TOKEN_T token);

//nikita
int SNS_start(char *filename);
int SNS_Assign_value(RULE_T ** rule, TOKEN_T * token);
enum RULE_TYPE_T SNS_Decide_rule_type(TOKEN_T * token);

void Error_MSG();
void WriteToken(TOKEN_T * token);
void Destruct_rule(RULE_T * rule);
void Error_msg();

#endif