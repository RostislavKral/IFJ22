//
// Created by jeza on 2.12.22.
//

#include "expression_stack.h"
void expr_stack_init(expStack* newStack){
    newStack->topItem = NULL;
    newStack->itemsCount = 0;
}
void expr_stack_push_to_top(expStack *stack, TOKEN_T token) {
    struct stackItem *newItem = malloc(sizeof(struct stackItem));
    newItem->token = token;
    newItem->tokenOperator = token.operators;
    newItem->tokenType = token.type;
    newItem->nextItem = stack->topItem;
    stack->topItem = newItem;
    stack->itemsCount++;
}
void expr_stack_push_to_bottom(expStack *stack, TOKEN_T token){
    struct stackItem *newItem = malloc(sizeof(struct stackItem));
    newItem->token = token;
    newItem->tokenType = token.type;
    newItem->tokenOperator = token.operators;
    newItem->nextItem = NULL;
    expStackItem *tmp = stack->topItem;
    while (tmp->nextItem != NULL){
        tmp = tmp->nextItem;
    }
    tmp->nextItem = newItem;
    stack->itemsCount++;
}
void expr_stack_pop(expStack* stack){
    expStackItem *tmp = stack->topItem;
    stack->topItem = stack->topItem->nextItem;
    free(tmp);
}

expStackItem* exp_stack_get_top(expStack* stack){
    return stack->topItem;
}

expStackItem* exp_stack_get_top_pop(expStack* stack){
    expStackItem *tmp;
    tmp = stack->topItem;
    expr_stack_pop(stack);
    return tmp;
}
expStackItem* exp_stack_get_bottom(expStack* stack){
    expStackItem *tmp;
    tmp = stack->topItem;
    while (tmp->nextItem != NULL){
        tmp = tmp->nextItem;
    }
    return tmp;
}