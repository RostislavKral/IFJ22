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

int table [17][17] = {
//                  NULL  *    /    +    -    .    <    >   <=   >=    =   ===  !==   (    )   ID    $
/*   NULL  */     { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
/*   *     */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   /     */     { ' ', '<', '<', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   +     */     { ' ', '<', '<', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   -     */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   .     */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   <     */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   >     */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   <=    */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   >=    */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   =     */     { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
/*   ===   */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   !==   */     { ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   (     */     { ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', '<', '<', '=', '<', ' ' },
/*   )     */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', ' ', '>', ' ', '>' },
/*   ID    */     { ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', ' ', '>', ' ', '>' },
/*   $     */     { ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', '<', '<', ' ', '<', ' ' }
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
    if(token->type == OPERATOR) return token->operators;
    if(token->type == LPAR)     return 13;
    if(token->type == RPAR)     return 14;
    if(token->type == TOKEN_ID) return 15;
    if(token->type == LITERAL)  return 15;
    if(token == NULL)           return 16;
    if(token->type == DOLLAR)   return 16;

    return -1;
}

int get_operator(DLList* stack, TOKEN_T* token)
{
    return table[get_row(stack)][get_column(token)];
}



int reduce(DLList* stack, Stack* stop_stack)
{
    StackItem* tmp = Stack_pop(stop_stack);


    while(tmp)
    {
        DLLItem* stop = tmp->item;
        int reduced = 0;

        if (stop->nextItem && stop->nextItem->nextItem && stop->nextItem->nextItem->nextItem)
        {
            // assert that expression is (E)
            if (
                    stop->nextItem                    ->token->type == LPAR &&
                   (stop->nextItem->nextItem          ->token->type == TOKEN_ID || stop->nextItem->nextItem->token->type == LITERAL) &&
                    stop->nextItem->nextItem->nextItem->token->type == RPAR
                    )
            {
                DLLItem* lpar = DLL_pop(stack, stop->nextItem);
                DLLItem* id = DLL_pop(stack, stop->nextItem);
                DLLItem* rpar = DLL_pop(stack, stop->nextItem);

                DLLItem* item = DLL_insert_after(stack, stop, id->token);
                item->bst = BST_make_tree_from_parentheses(lpar, id);

                free(lpar);
                free(id);
                free(rpar);

                reduced = 1;
            }
                // Rule E -> E + E  (OR ANY BINARY OPERATION)
            else if (
                   (stop->nextItem                    ->token->type == TOKEN_ID || stop->nextItem->token->type == LITERAL) &&
                    stop->nextItem->nextItem          ->token->type == OPERATOR &&
                   (stop->nextItem->nextItem->nextItem->token->type == TOKEN_ID || stop->nextItem->nextItem->nextItem->token->type == LITERAL)
                    )
            {
                DLLItem* id = DLL_pop(stack, stop->nextItem);
                DLLItem* operator = DLL_pop(stack, stop->nextItem);
                DLLItem* id2 = DLL_pop(stack, stop->nextItem);

                int type = validate_expression(id, operator, id2);
                if(type == 0)
                {
                    // expression is not valid
                    free(id);
                    free(operator);
                    free(id2);
                    return 0;
                }

                DLLItem* item = DLL_insert_after(stack, stop, id->token);
                item->bst = BST_make_tree_from_expression(id, operator, id2);

                free(id);
                free(operator);
                free(id2);

                reduced = 1;
            }
        }
        else if (stop->nextItem->token->type == TOKEN_ID || stop->nextItem->token->type == LITERAL)
        {
            reduced = 1;
        }

        if (reduced == 0)
        {
            Stack_push(stop_stack, tmp->item);
            break;
        }
        free(tmp);
        tmp = Stack_pop(stop_stack);
    }
    return 1;
}

BSTnode* analyze_precedence(DLList* list)
{
    DLLItem* iterator = list->first;
    DLLItem* stop = NULL;

    DLList* stack = malloc(sizeof (DLList));
    DLL_init(stack);
    DLL_insert_first(stack, create_dollar_token());

    Stack* stop_stack = malloc(sizeof (Stack));
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
            return NULL;
        }

        iterator = iterator->nextItem;
    }
    if(reduce(stack, stop_stack) == 0)
    {
        return NULL;
    }
//    printf("end:\n");
//    printf("expression: "); DLL_print(list);
//    printf("stack: "); DLL_print(stack);
//    printf("stop stack: "); Stack_print(stop_stack);

    DLLItem* item = DLL_pop_last(stack);
    DLLItem* dollar = DLL_pop_last(stack);
    if (
            (item->token->type == TOKEN_ID || item->token->type == LITERAL) &&
            dollar->token->type == DOLLAR &&
            stack->first == NULL)
    {
        BSTnode* node = item->bst;
        free(item);
        free(dollar);
        return node;
    }

//    free(item);
//    free(dollar);
//    return 1;
    return NULL;
}

TOKEN_T* create_dollar_token()
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = DOLLAR;

    return token;
}
int validate_expression(DLLItem* a, DLLItem* operator, DLLItem* b)
{
    if (operator->token->type != OPERATOR)
    {
        return 0;
    }
    if (a->token->type != TOKEN_ID && a->token->type != LITERAL)
    {
        return 0;
    }
    if (b->token->type != TOKEN_ID && b->token->type != LITERAL)
    {
        return 0;
    }
//    printf("AHOOOOOOOJ %d %d %d", a->token->value.type, operator->token->operators, b->token->value.type);
    if (
            operator->token->operators == MULTIPLY ||
            operator->token->operators == DIVIDE ||
            operator->token->operators == PLUS ||
            operator->token->operators == MINUS
            )
    {
        if (
                (a->token->value.type == 0 || a->token->value.type == 2) &&
                (b->token->value.type == 0 || b->token->value.type == 2)
                )
        {
            if (a->token->value.type == 0 && b->token->value.type == 0)
            {
                // TODO: is int
                return 1;
            }
            // TODO: is double
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (operator->token->operators == LESS ||
             operator->token->operators == GREATER ||
             operator->token->operators == LESS_EQUAL ||
             operator->token->operators == GREATER_EQUAL ||
             operator->token->operators == TYPE_EQUALS ||
             operator->token->operators == TYPE_NOT_EQUALS
             )
    {
        if (
                (a->token->value.type == 0 || a->token->value.type == 2) &&
                (b->token->value.type == 0 || b->token->value.type == 2)
                )
        {
            // TODO: is boolean
            return 1;
        }
    }
    else if (operator->token->operators == CONCAT)
    {
        if (
                a->token->value.type == 1 &&
                b->token->value.type == 1
                )
        {
            // TODO: is string
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}