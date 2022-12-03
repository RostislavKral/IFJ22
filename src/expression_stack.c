//
// Created by jeza on 2.12.22.
//

#include "expression_stack.h"
void DLL_init(DLList* list){
    list->first = NULL;
    list->last = NULL;
}
void DLL_insert_first(DLList *list, TOKEN_T* token) {
    struct DLLItem *newItem = malloc(sizeof(struct DLLItem));
    newItem->token = token;
    newItem->nextItem = list->first;
    newItem->prevItem = NULL;
    list->first = newItem;
    list->itemsCount++;
}
void DLL_insert_last(DLList *list, TOKEN_T* token){
    struct DLLItem *newItem = malloc(sizeof(struct DLLItem));
    newItem->token = token;
    newItem->nextItem = NULL;
    newItem->prevItem = list->last;
    list->last = newItem;
    list->itemsCount++;
}
DLLItem* DLL_pop_first(DLList* list){
    DLLItem* result = list->first;
    list->first = result->nextItem;
    return result;
}

DLLItem* DLL_pop_last(DLList* list){
    DLLItem* result = list->last;
    list->last = result->prevItem;
    return result;
}

DLLItem* DLL_get_first(DLList* list){
    return list->first;
}

DLLItem* stack_get_bottom(DLList* list){
    return list->last;
}