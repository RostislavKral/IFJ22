/**
 * @file expressions_stack.h
 * @author Hoang Nam Nguyen (xnguye22)
 * @brief Expressions structures
 */

#ifndef IFJ22_EXPRESSION_STACK_H
#define IFJ22_EXPRESSION_STACK_H
#include "stdlib.h"
#include "stdio.h"
#include "token.h"

typedef struct BSTnode{
    TOKEN_T* token;
    enum T_KEYWORD type;
    struct BSTnode* left;
    struct BSTnode* right;
} BSTnode;

typedef struct DLLItem{
    enum T_OPERATOR tokenOperator;
    enum T_TOKEN_TYPE tokenType;
    TOKEN_T *token;
    struct DLLItem *prevItem;
    struct DLLItem *nextItem;

    BSTnode *bst;
} DLLItem;

typedef struct DLList {
    int itemsCount;
    DLLItem *first;
    DLLItem *last;
} DLList;

void DLL_init(DLList* list);
void DLL_dispose_list(DLList* list);
DLLItem* DLL_insert_first(DLList* list, TOKEN_T* token);
DLLItem* DLL_insert_last(DLList* list, TOKEN_T* token);
DLLItem* DLL_insert_after(DLList *list, DLLItem *item, TOKEN_T* token);
DLLItem* DLL_pop_first(DLList* list);
DLLItem* DLL_pop_last(DLList* list);
DLLItem* DLL_pop(DLList *list, DLLItem *item);
DLLItem* DLL_get_first(DLList* list);
DLLItem* DLL_get_last(DLList* list);
DLList* DLL_copy(DLList* list);
void DLL_print(DLList* list);
void DLL_item_print(DLLItem* item);


typedef struct StackItem{
    DLLItem* item;
    struct StackItem* next;
} StackItem;


typedef struct Stack {
    int itemsCount;
    StackItem* first;
} Stack;

void Stack_init(Stack* stack);
StackItem* Stack_push(Stack* stack, DLLItem* item);
StackItem* Stack_pop(Stack* stack);
void Stack_print(Stack* stack);

BSTnode* BST_init();
void BST_dispose(BSTnode* root);
BSTnode* BST_init_token(DLLItem* item);
BSTnode* BST_make_tree_from_expression(DLLItem* a, DLLItem* operator, DLLItem* b);
BSTnode* BST_make_tree_from_parentheses(DLLItem* parentheses, DLLItem* id);
void BST_print(BSTnode* root, int level);

#endif //IFJ22_EXPRESSION_STACK_H
