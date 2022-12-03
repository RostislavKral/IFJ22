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
#include "expression_stack.h"
#include "expression_stack.c"
#endif //IFJ22_EXPRESSIONS_H

void analyze_precedence(DLList* list);
