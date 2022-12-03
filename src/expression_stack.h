//
// Created by jeza on 2.12.22.
//

#ifndef IFJ22_EXPRESSION_STACK_H
#define IFJ22_EXPRESSION_STACK_H
#include "stdlib.h"
#include "stdio.h"
#include "token.h"


typedef struct DLLItem{
    enum T_OPERATOR tokenOperator;
    enum T_TOKEN_TYPE tokenType;
    TOKEN_T *token;
    struct DLLItem *prevItem;
    struct DLLItem *nextItem;
}DLLItem;

typedef struct DLList {
    int itemsCount;
    DLLItem *first;
    DLLItem *last;
} DLList;

void DLL_init(DLList* list);
void DLL_insert_first(DLList* list, TOKEN_T* token);
void DLL_insert_last(DLList* list, TOKEN_T* token);
DLLItem* DLL_pop_first(DLList* list);
DLLItem* DLL_pop_last(DLList* list);
DLLItem* DLL_get_first(DLList* list);
DLLItem* DLL_get_last(DLList* list);
#endif //IFJ22_EXPRESSION_STACK_H
