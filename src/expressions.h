/**
 * @file expressions.h
 * @author Hoang Nam Nguyen (xnguye22)
 * @brief Expressions processing
 */

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
#include "data_structures.h"

int get_column(TOKEN_T* token);
int get_row(DLList* list);
BSTnode* analyze_precedence(DLList* list);
int get_operator(DLList* stack, TOKEN_T* token);
TOKEN_T* create_dollar_token();
enum T_KEYWORD validate_expression(DLLItem* a, DLLItem* operator, DLLItem* b);

#endif //IFJ22_EXPRESSIONS_H