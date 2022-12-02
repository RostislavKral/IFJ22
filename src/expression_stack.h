//
// Created by jeza on 2.12.22.
//

#ifndef IFJ22_EXPRESSION_STACK_H
#define IFJ22_EXPRESSION_STACK_H
#include "stdlib.h"
#include "stdio.h"
#include "token.h"


typedef struct stackItem{
    enum T_OPERATOR tokenOperator;
    enum T_TOKEN_TYPE tokenType;
    TOKEN_T token;
    struct stackItem *nextItem;
}expStackItem;

typedef struct stack {
    int itemsCount;
    expStackItem *topItem;
} expStack;

void expr_stack_init(expStack* newStack);
void expr_stack_push_to_top(expStack *stack, TOKEN_T token);
void expr_stack_push_to_bottom(expStack *stack, TOKEN_T token);
void exp_stack_pop(expStack* stack);
expStackItem* exp_stack_get_top(expStack* stack);
expStackItem* exp_stack_get_top_pop(expStack* stack);
expStackItem* exp_stack_get_bottom(expStack* stack);
#endif //IFJ22_EXPRESSION_STACK_H
