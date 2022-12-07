/**
 * @file expressions.c
 * @author Hoang Nam Nguyen (xnguye22)
 * @brief Expressions processing
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "symtable.h"
#include "lexer.h"
#include "token.h"
#include "expressions.h"
#include "expression_stack.h"
#include "codegen.h"


int table [18][18] = {
//                  NULL  *    /    +    -    .    <    >   <=   >=    =   !=   ===  !==   (    )   ID    $
/*   NULL  */     { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
/*   *     */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', '>', '<', '>', '<', '>' },
/*   /     */     { ' ', '<', '<', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', '>', '<', '>', '<', '>' },
/*   +     */     { ' ', '<', '<', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', '>', '<', '>', '<', '>' },
/*   -     */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', '>', '<', '>', '<', '>' },
/*   .     */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', '>', '<', '>', '<', '>' },
/*   <     */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   >     */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   <=    */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   >=    */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   =     */     { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
/*   !=    */     { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
/*   ===   */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   !==   */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   (     */     { ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', ' ', '<', '<', '<', '=', '<', ' ' },
/*   )     */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', '>', ' ', '>', ' ', '>' },
/*   ID    */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', '>', '>', ' ', '>', ' ', '>' },
/*   $     */     { ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', ' ', '<', '<', '<', ' ', '<', ' ' }
};

int get_row(DLList* stack)
{
    if (stack->last == NULL)
    {
        return 15;
    }
    return get_column(stack->last->token);
}

int get_column(TOKEN_T* token)
{
    if(token == NULL)           return 17;

    if(token->type == OPERATOR) return token->operators;
    if(token->type == LPAR)     return 14;
    if(token->type == RPAR)     return 15;

    if(token->type == TOKEN_ID) return 16;
    if(token->type == LITERAL)  return 16;
    if(token->keyword == KEY_NULL)  return 16;

    if(token->type == DOLLAR)   return 17;

    return -1;
}

int get_operator(DLList* stack, TOKEN_T* token)
{
    return table[get_row(stack)][get_column(token)];
}

int get_priority(TOKEN_T* operator)
{
    if (operator == NULL) return 0;

    if (operator->operators == MULTIPLY)            return 1;
    if (operator->operators == DIVIDE)              return 1;
    if (operator->operators == PLUS)                return 2;
    if (operator->operators == MINUS)               return 2;
    if (operator->operators == CONCAT)              return 2;
    if (operator->operators == LESS)                return 3;
    if (operator->operators == GREATER)             return 3;
    if (operator->operators == LESS_EQUAL)          return 3;
    if (operator->operators == GREATER_EQUAL)       return 3;
    if (operator->operators == TYPE_EQUALS)         return 4;
    if (operator->operators == TYPE_NOT_EQUALS)     return 4;

    return 0;
}

int compare_operators(TOKEN_T* operator, TOKEN_T* operator_stack)
{
    if (operator->type != OPERATOR && operator_stack->type != OPERATOR)
    {
        return -1;
    }

    int priority = get_priority(operator);
    int priority_stack = get_priority(operator_stack);

    return priority > priority_stack;
}

bool reduce(DLList* stack, Stack* stop_stack)
{
    StackItem* tmp = Stack_pop(stop_stack);

    if (!DLL_has_value(stack, tmp->item->token))
    {
        return true;
    }

    DLLItem* stop = tmp->item;
    int reduced = 0;

    if (stop->nextItem && stop->nextItem->nextItem && stop->nextItem->nextItem->nextItem)
    {
        // assert that expression is (E)
        if (
                stop->nextItem                    ->token->type == LPAR &&
               (stop->nextItem->nextItem          ->token->type == TOKEN_ID || stop->nextItem->nextItem->token->type == LITERAL || stop->nextItem->nextItem->token->keyword == KEY_NULL) &&
                stop->nextItem->nextItem->nextItem->token->type == RPAR
                )
        {
            DLLItem* lpar = DLL_pop(stack, stop->nextItem);
            DLLItem* id = DLL_pop(stack, stop->nextItem);
            DLLItem* rpar = DLL_pop(stack, stop->nextItem);

            DLLItem* item = DLL_insert_after(stack, stop, id->token);
//            item->bst = BST_make_tree_from_parentheses(lpar, id);
            item->bst = id->bst;

            free(lpar);
            free(id);
            free(rpar);

            reduced = 1;
        }
            // Rule E -> E + E  (OR ANY BINARY OPERATION)
        else if (
               (stop->nextItem                    ->token->type == TOKEN_ID || stop->nextItem                    ->token->type == LITERAL || stop->nextItem->token->keyword == KEY_NULL) &&
                stop->nextItem->nextItem          ->token->type == OPERATOR &&
               (stop->nextItem->nextItem->nextItem->token->type == TOKEN_ID || stop->nextItem->nextItem->nextItem->token->type == LITERAL || stop->nextItem->nextItem->nextItem->token->keyword == KEY_NULL)
                )
        {
            // get previous operator
            // compare operators
            // if this operator is higher, process, otherwise skip
            // if I get ( before operator, process

            DLLItem* id = DLL_pop(stack, stop->nextItem);
            DLLItem* operator = DLL_pop(stack, stop->nextItem);
            DLLItem* id2 = DLL_pop(stack, stop->nextItem);

            DLLItem* iterator = stop;
            while (iterator)
            {
                if (iterator->token->type == DOLLAR)
                {
                    DLLItem* item = DLL_insert_after(stack, stop, id->token);
                    item->bst = BST_make_tree_from_expression(id, operator, id2);
                    item->bst->type = validate_expression(id, operator, id2);

                    break;
                }
                if (iterator->token->type == LPAR)
                {
                    DLLItem* item = DLL_insert_after(stack, stop, id->token);
                    item->bst = BST_make_tree_from_expression(id, operator, id2);
                    item->bst->type = validate_expression(id, operator, id2);

//                        free(id);
//                        free(operator);
//                        free(id2);
                    break;
                }
                if (iterator->token->type == OPERATOR && compare_operators(operator->token, iterator->token))
                {
                    DLL_insert_after(stack, stop, id2->token)->bst = id2->bst;
                    DLL_insert_after(stack, stop, operator->token)->bst = operator->bst;
                    DLL_insert_after(stack, stop, id->token)->bst = id->bst;

                    Stack_push(stop_stack, tmp->item);
                    Stack_push(stop_stack, iterator->prevItem->prevItem);
                    return true;
                }
                if (iterator->token->type == OPERATOR && !compare_operators(operator->token, iterator->token))
                {
                    DLLItem* item = DLL_insert_after(stack, stop, id->token);
                    item->bst = BST_make_tree_from_expression(id, operator, id2);
                    item->bst->type = validate_expression(id, operator, id2);

//                        free(id);
//                        free(operator);
//                        free(id2);
                    break;
                }
                iterator = iterator->prevItem;
            }

            reduced = 1;
        }
    }
    else if (stop->nextItem->token->type == TOKEN_ID || stop->nextItem->token->type == LITERAL || stop->nextItem->token->keyword == KEY_NULL)
    {
        DLLItem* item = stop->nextItem;
        if (!item->bst)
        {
            item->bst = BST_init_token(item);
        }
        reduced = 1;
    }

    if (reduced == 0)
    {
//            Stack_push(stop_stack, tmp->item);
        return true;
    }
    free(tmp);
//        tmp = Stack_pop(stop_stack);
    return true;
}

BSTnode* analyze_precedence(DLList* list)
{
    DLLItem* iterator = list->first;
    DLLItem* stop = NULL;

    DLList* stack = malloc(sizeof (DLList));
    if (stack == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);

    DLL_init(stack);
    DLL_insert_first(stack, create_dollar_token());

    Stack* stop_stack = malloc(sizeof (Stack));
    if (stop_stack == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);

    Stack_init(stop_stack);

    while (iterator)
    {
//        printf("expression: "); DLL_print(list);
//        printf("stack: "); DLL_print(stack);
//        printf("stop stack: "); Stack_print(stop_stack);
//        //printf("stack_size: %d", stack->itemsCount);
//        printf("token_type: %d, token: %d\n", iterator->token->type,  iterator->token->operators);
//        printf("row: %d, column: %d\n", get_row(stack), get_column(iterator->token));
//        printf("operation: %c\n", table[get_row(stack)][get_column(iterator->token)]);
//        printf("=========================================\n");

        if (table[get_row(stack)][get_column(iterator->token)] == '=')
        {
            DLL_insert_last(stack, iterator->token);
        }
        else if (table[get_row(stack)][get_column(iterator->token)] == '<')
        {
            stop = DLL_insert_last(stack, iterator->token)->prevItem;
            Stack_push(stop_stack, stop);
        }
        else if (table[get_row(stack)][get_column(iterator->token)] == '>')
        {
            if(reduce(stack, stop_stack) == 0)
            {
                return NULL;
            }
            stop = DLL_insert_last(stack, iterator->token)->prevItem->prevItem;
            Stack_push(stop_stack, stop);
        }
        else if (table[get_row(stack)][get_column(iterator->token)] == ' ')
        {
            // return -1;
            exit_with_message(iterator->token->lineNum, iterator->token->charNum, "Invalid expression", SYNTAX_ERR);
            return NULL;
        }

        iterator = iterator->nextItem;
    }
    while(stop_stack->first) {
        if (reduce(stack, stop_stack) == false) {
            return NULL;
        }
    }

    Stack_push(stop_stack, stack->first);
    if (reduce(stack, stop_stack) == false) {
        return NULL;
    }
    Stack_push(stop_stack, stack->first);
    if (reduce(stack, stop_stack) == false) {
        return NULL;
    }
//    printf("end:\n");
//    printf("expression: "); DLL_print(list);
//    printf("stack: "); DLL_print(stack);
//    printf("stop stack: "); Stack_print(stop_stack);

    DLLItem* item = DLL_pop_last(stack);
    DLLItem* dollar = DLL_pop_last(stack);
    if (
            (item->token->type == TOKEN_ID || item->token->type == LITERAL || item->token->keyword == KEY_NULL) &&
            dollar->token->type == DOLLAR &&
            stack->first == NULL)
    {
        BSTnode* node = item->bst;

        free(item);
        free(dollar);
        return node;
    }
//    DLLItem* dollar = DLL_pop_last(stack);
//    if (dollar->token->type == DOLLAR && stack->first == NULL)
//    {
//        free(dollar);
//        printf("JUPI");
//        return NULL;
//    }

//    free(item);
//    free(dollar);
//    return 1;
    return NULL;
}

TOKEN_T* create_dollar_token()
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));

    if (token == NULL) exit_with_message(0,0, "malloc failed", GENERAL_ERR);

    token->type = DOLLAR;

    return token;
}

enum T_KEYWORD validate_expression(DLLItem* a, DLLItem* operator, DLLItem* b)
{
    if (operator->token->type != OPERATOR)
    {
        return KEY_NULL;
    }
    if (a->token->type != TOKEN_ID && a->token->type != LITERAL && a->token->keyword != KEY_NULL)
    {
        return KEY_NULL;
    }
    if (b->token->type != TOKEN_ID && b->token->type != LITERAL && a->token->keyword != KEY_NULL)
    {
        return KEY_NULL;
    }
//    printf("AHOOOOOOOJ %d %d %d", a->token->value.type, operator->token->operators, b->token->value.type);
    if (
            operator->token->operators == MULTIPLY ||
            operator->token->operators == PLUS ||
            operator->token->operators == MINUS
            )
    {
        if (
                (a->token->value.type == 0 || a->token->value.type == 2) &&
                (b->token->value.type == 0 || b->token->value.type == 2) &&
                (a->token->keyword != KEY_NULL && b->token->keyword != KEY_NULL)
                )
        {
            if (a->token->value.type == 0 && b->token->value.type == 0)
            {
                // is int
                return KEY_INT;
            }
            // is double
            return KEY_FLOAT;
        }
        else
        {
            return KEY_NULL;
        }
    }
    else if(operator->token->operators == DIVIDE)
    {
        if (
                (a->token->value.type == 0 || a->token->value.type == 2) &&
                (b->token->value.type == 0 || b->token->value.type == 2)
                )
        {
            if (a->token->value.type == 0 && b->token->value.type == 0)
            {
                return KEY_INT;
            }
            return KEY_FLOAT;
        }
        else
        {
            return KEY_NULL;
        }
    }
    else if (operator->token->operators == LESS ||
             operator->token->operators == GREATER ||
             operator->token->operators == LESS_EQUAL ||
             operator->token->operators == GREATER_EQUAL
             )
    {
        return KEY_INT;
        if (
                (a->token->value.type == 0 || a->token->value.type == 2) &&
                (b->token->value.type == 0 || b->token->value.type == 2)
                )
        {
            // is boolean
//            return KEY_BOOLEAN;
            return KEY_INT;
        }
    }
    else if(
            operator->token->operators == TYPE_EQUALS ||
            operator->token->operators == TYPE_NOT_EQUALS)
    {
//        return KEY_BOOLEAN;
        return KEY_INT;
    }
    else if (operator->token->operators == CONCAT)
    {
        if (
                a->token->value.type == 1 &&
                b->token->value.type == 1
                )
        {
            // is string
            return KEY_STRING;
        }
        else
        {
            return KEY_NULL;
        }
    }

    return KEY_NULL;
}