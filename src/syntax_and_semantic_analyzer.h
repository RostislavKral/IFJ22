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
#include "expressions.h"
#include "token.h"
#include "expression_stack.h"
//#include "codegen.h"

typedef struct {
    bool fParsing;
    bool fHeadParsed;
    bool fNamePass;
    bool fParamPass;
    bool fBodyParsing;
    bool fReturnTypePass;
    int fParamCount;
    int fBraceCountCheck;
    char * name;
    DLList* paramsList;
    enum T_KEYWORD returnType;
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
    //Check if already <?php is parsed
    bool isDefined;
    //num of opened ifs
    int openedIfCount;
} scopeHelper;

void analyze_token(htab_t* symtable);
void function_detected(TOKEN_T* token,htab_t* symtable);
void function_end_parsing();
void if_condition(TOKEN_T* token, htab_t* symtable);
void while_condition(TOKEN_T* token, htab_t* symtable);
void var_declaration(htab_t* symtable,TOKEN_T *variable);
void function_call(TOKEN_T *funcName,htab_t* symtable);
DLList* expression_list(htab_t* symtable, enum  T_TOKEN_TYPE closingToken);


void WriteToken(TOKEN_T * token);

#endif