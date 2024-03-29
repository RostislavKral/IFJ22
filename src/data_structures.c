/**
 * @file expressions_stack.c
 * @author Hoang Nam Nguyen (xnguye22)
 * @brief Expressions structures
 */

#include <stdbool.h>
#include <string.h>
#include "data_structures.h"
#include "token.h"
#include "stdlib.h"
#include "stdio.h"

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
bool DLL_find_token(DLList *list, TOKEN_T* token){
    DLLItem *item = DLL_get_first(list);
    while (item != NULL){
        if(strcmp(item->token->name,token->name) == 0) return true;
        item = item->nextItem;
    }
    return false;
}
DLLItem* DLL_insert_first(DLList *list, TOKEN_T* token) {
    DLLItem *newItem = malloc(sizeof(DLLItem));
    if (newItem == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);
    newItem->bst = NULL;
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
    if (newItem == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);
    newItem->bst = NULL;
    newItem->token = token;
    newItem->nextItem = NULL;
    newItem->prevItem = list->last;

    if (list->itemsCount == 0)
    {
        list->first = newItem;
    }
    else
    {
        newItem->prevItem->nextItem = newItem;
    }

    list->last = newItem;
    list->itemsCount++;

    return newItem;
}

DLLItem* DLL_insert_after(DLList *list, DLLItem *item, TOKEN_T* token){
    DLLItem *newItem = malloc(sizeof(DLLItem));
    if (newItem == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);
    newItem->bst = NULL;
    newItem->token = token;
    newItem->prevItem = item;
    newItem->nextItem = item->nextItem;
    item->nextItem = newItem;

    if (newItem->nextItem)
    {
        newItem->nextItem->prevItem = newItem;
    }
    else
    {
        list->last = newItem;
    }

    if (list->itemsCount == 1)
    {
        list->last = newItem;
    }
    list->itemsCount++;

    return newItem;
}

DLLItem* DLL_pop(DLList *list, DLLItem *item){
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
    if (newList == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);
    newList->itemsCount = list->itemsCount;

    DLLItem* item = list->first;

    while(item)
    {
        DLL_insert_last(newList, item->token);
        item = item->nextItem;
    }

    return newList;
}

int DLL_has_value(DLList* list, TOKEN_T* token)
{
    DLLItem* item = list->first;
    while(item)
    {
        if (item->token == token)
        {
            return 1;
        }
        item = item->nextItem;
    }
    return 0;
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
    else if (item->token->type == LPAR)
    {
        printf("(");
    }
    else if (item->token->type == RPAR)
    {
        printf(")");
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
    if (newItem == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);
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

BSTnode* BST_init()
{
    BSTnode* root = malloc(sizeof(BSTnode));
    if (root == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);

    root->left = NULL;
    root->right = NULL;
    root->token = NULL;
    root->type = KEY_NULL;

    return root;
}

BSTnode* BST_init_token(DLLItem* item)
{
    BSTnode* root = malloc(sizeof(BSTnode));
    if (root == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);

    root->left = NULL;
    root->right = NULL;
    root->token = item->token;
    root->type = get_keyword_from_token(item->token);

    return root;
}

BSTnode* BST_make_tree_from_expression(DLLItem* a, DLLItem* operator, DLLItem* b)
{
    BSTnode* root = BST_init_token(operator);

    if (a->bst) root->left = a->bst;
    else root->left = a->bst;

    if (b->bst) root->right = b->bst;
    else root->right = b->bst;

    return root;
}

BSTnode* BST_make_tree_from_parentheses(DLLItem* parentheses, DLLItem* id)
{
    BSTnode* root = BST_init_token(parentheses);

    if (id->bst) root->left = id->bst;
    else root->left = BST_init_token(id);

    return root;
}


void BST_print(BSTnode* root, int level)
{
    if (root == NULL)
        return;


    BST_print(root->left, level + 1);

    for (int i = 0; i < level; i++)
        printf("  ");
    token_print(root->token);
    printf("\n");

    BST_print(root->right, level + 1);
}

void BST_dispose(BSTnode* root)
{
    if (root->left) BST_dispose(root->left);
    if (root->right) BST_dispose(root->right);

    free(root);
}