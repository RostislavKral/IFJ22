//
// Created by jeza on 2.12.22.
//

#include "expression_stack.h"

void DLL_init(DLList* list){
    list->itemsCount = 0;
    list->first = NULL;
    list->last = NULL;
}

void DLL_dispose_list(DLList* list){
    DLLItem *tmp;
    while (list->first != NULL){
        tmp = list->first;
        list->first = list->first->nextItem;
        free(tmp);
    }
}

DLLItem* DLL_insert_first(DLList *list, TOKEN_T* token) {
    DLLItem *newItem = malloc(sizeof(DLLItem));
    newItem->token = token;
    newItem->nextItem = list->first;
    newItem->prevItem = NULL;

    if (list->itemsCount == 0)
    {
        list->last = newItem;
    }
    else
    {
        newItem->nextItem->prevItem = newItem;
    }

    list->first = newItem;
    list->itemsCount++;

    return newItem;
}
DLLItem* DLL_insert_last(DLList *list, TOKEN_T* token){
    DLLItem *newItem = malloc(sizeof(DLLItem));
    newItem->token = token;
    newItem->nextItem = NULL;
    newItem->prevItem = list->last;

    if (newItem->prevItem)
    {
        newItem->prevItem->nextItem = newItem;
    }

    if (list->itemsCount == 0)
    {
        list->first = newItem;
    }

    list->last = newItem;
    list->itemsCount++;

    return newItem;
}

DLLItem* DLL_insert_after(DLList *list, DLLItem *item, TOKEN_T* token){
    DLLItem *newItem = malloc(sizeof(DLLItem));
    newItem->token = token;
    newItem->prevItem = item;
    newItem->nextItem = item->nextItem;
    item->nextItem = newItem;

    if (newItem->nextItem)
    {
        newItem->nextItem->prevItem = newItem;
    }

    if (list->itemsCount == 1)
    {
        list->last = newItem;
    }
    list->itemsCount++;

    return newItem;
}

DLLItem* DLL_pop_after(DLList *list, DLLItem *item){
    if (item->nextItem)
    {
        item->nextItem->prevItem = item->prevItem;
    }
    if (item->prevItem)
    {
        item->prevItem->nextItem = item->nextItem;
    }

    list->itemsCount--;
    if (list->itemsCount == 1)
    {
        list->last = list->first;
    }

    return item;
}

DLLItem* DLL_pop_first(DLList* list){
    DLLItem* result = list->first;
    list->first = result->nextItem;
    list->itemsCount--;

    if (list->itemsCount == 0)
    {
        list->last = NULL;
    }

    return result;
}

DLLItem* DLL_pop_last(DLList* list){
    DLLItem* result = list->last;
    list->last = result->prevItem;
    list->itemsCount--;

    if (list->itemsCount == 0)
    {
        list->first = NULL;
    }

    return result;
}

DLLItem* DLL_get_first(DLList* list){
    return list->first;
}

DLLItem* DLL_get_last(DLList* list){
    return list->last;
}

DLList* DLL_copy(DLList* list)
{
    DLList* newList = malloc(sizeof (DLList*));
    newList->itemsCount = list->itemsCount;

    DLLItem* item = list->first;

    while(item)
    {
        DLL_insert_last(newList, item->token);
        item = item->nextItem;
    }

    return newList;
}

void DLL_item_print(DLLItem* item)
{
    if (item->token->type == TOKEN_ID)
    {
        printf("%s", item->token->name);
    }
    else if (item->token->type == DOLLAR)
    {
        printf("$");
    }
    else if (item->token->operators == MULTIPLY)
    {
        printf("*");
    }
    else if (item->token->operators == DIVIDE)
    {
        printf("/");
    }
    else if (item->token->operators == PLUS)
    {
        printf("+");
    }
    else if (item->token->operators == MINUS)
    {
        printf("-");
    }
    else if (item->token->operators == CONCAT)
    {
        printf(".");
    }
    else if (item->token->operators == LESS)
    {
        printf("<");
    }
    else if (item->token->operators == GREATER)
    {
        printf(">");
    }
    else if (item->token->operators == LESS_EQUAL)
    {
        printf("<=");
    }
    else if (item->token->operators == GREATER_EQUAL)
    {
        printf(">=");
    }
    else if (item->token->operators == EQUALS)
    {
        printf("=");
    }
    else if (item->token->operators == NOT_EQUAL)
    {
        printf("!=");
    }
    else if (item->token->operators == TYPE_EQUALS)
    {
        printf("===");
    }
    else if (item->token->operators == TYPE_NOT_EQUALS)
    {
        printf("!==");
    }
}

void DLL_print(DLList* list)
{
    DLLItem* item = list->first;
    while(item)
    {
        DLL_item_print(item);
        if (item->nextItem)
        {
            printf(" -> ");
        }
        item = item->nextItem;
    }
    printf("\n");
}

void Stack_init(Stack* stack)
{
    stack->itemsCount = 0;
    stack->first = NULL;
}

StackItem* Stack_push(Stack* stack, DLLItem* item)
{
    StackItem *newItem = malloc(sizeof(StackItem));
    newItem->item = item;
    newItem->next = stack->first;

    stack->first = newItem;
    stack->itemsCount++;
    return newItem;
}

StackItem* Stack_pop(Stack* stack)
{
    StackItem* result = stack->first;
    if (result == NULL)
    {
        return NULL;
    }

    stack->first = result->next;
    stack->itemsCount--;
    return result;
}

void Stack_print(Stack* stack)
{
    StackItem* item = stack->first;
    while(item)
    {
        DLL_item_print(item->item);
        if (item->next)
        {
            printf(" -> ");
        }
        item = item->next;
    }
    printf("\n");
}