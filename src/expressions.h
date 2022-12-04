//
// Created by jeza on 2.12.22.
//

#ifndef IFJ22_EXPRESSIONS_H
#define IFJ22_EXPRESSIONS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "symtable.h"
#include "lexer.h"
#include "token.h"
#include "expressions.h"
#include "expression_stack.h"

int get_column(TOKEN_T* token);
int get_row(DLList* list);
void analyze_precedence(DLList* list);
char get_operator(DLList* stack, TOKEN_T* token);
TOKEN_T* create_dollar_token();

#endif //IFJ22_EXPRESSIONS_H