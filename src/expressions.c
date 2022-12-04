//
// Created by jeza on 2.12.22.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "symtable.h"
#include "lexer.h"
#include "token.h"
#include "expressions.h"
#include "expression_stack.h"

int table [16][16] = {
//                   *    /    +    -    .    <    >   <=   >=    =   ===  !==   (    )   ID    $
/*   *     */     { '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   /     */     { '<', '<', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   +     */     { '<', '<', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   -     */     { '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   .     */     { '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '<', '>', '<', '>' },
/*   <     */     { '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   >     */     { '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   <=    */     { '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   >=    */     { '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   =     */     { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
/*   ===   */     { '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   !==   */     { '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', '>', '<', '>' },
/*   (     */     { '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', '<', '<', '=', '<', ' ' },
/*   )     */     { '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', ' ', '>', ' ', '>' },
/*   ID    */     { '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', ' ', '>', ' ', '>' },
/*   $     */     { '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', '<', '<', ' ', '<', ' ' }
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
    if(token->type == LPAR)     return 12;
    if(token->type == RPAR)     return 13;
    if(token->type == TOKEN_ID) return 14;
    if(token == NULL)           return 15;
    if(token->type == DOLLAR)   return 15;

    return -1;
}

char get_operator(DLList* stack, TOKEN_T* token)
{
    return table[get_row(stack)][get_column(token)];
}

void reduce(DLList* stack, Stack* stop_stack)
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
                    stop->nextItem->nextItem          ->token->type == TOKEN_ID &&
                    stop->nextItem->nextItem->nextItem->token->type == RPAR
                    )
            {
                DLLItem* lpar = DLL_pop(stack, stop->nextItem);
                DLLItem* id = DLL_pop(stack, stop->nextItem);
                DLLItem* rpar = DLL_pop(stack, stop->nextItem);

                DLL_insert_after(stack, stop, id->token);

                free(lpar);
                free(id);
                free(rpar);

                reduced = 1;
            }
                // Rule E -> E + E  (OR ANY BINARY OPERATION)
            else if (
                    stop->nextItem                    ->token->type == TOKEN_ID &&
                    stop->nextItem->nextItem          ->token->type == OPERATOR &&
                    stop->nextItem->nextItem->nextItem->token->type == TOKEN_ID
                    )
            {
                DLLItem* id = DLL_pop(stack, stop->nextItem);
                DLLItem* operator = DLL_pop(stack, stop->nextItem);
                DLLItem* id2 = DLL_pop(stack, stop->nextItem);

                DLL_insert_after(stack, stop, id->token);

                free(id);
                free(operator);
                free(id2);

                reduced = 1;
            }
        }
        else if (stop->nextItem->token->type == TOKEN_ID)
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
}

int analyze_precedence(DLList* list)
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
            reduce(stack, stop_stack);
            stop = DLL_insert_last(stack, iterator->token)->prevItem->prevItem;
            Stack_push(stop_stack, stop);
        }
        else if (table[get_row(stack)][get_column(iterator->token)] == ' ')
        {
            return -1;
        }

        iterator = iterator->nextItem;
    }
    reduce(stack, stop_stack);
//    printf("end:\n");
//    printf("expression: "); DLL_print(list);
//    printf("stack: "); DLL_print(stack);
//    printf("stop stack: "); Stack_print(stop_stack);

    DLLItem* item = DLL_pop_last(stack);
    DLLItem* dollar = DLL_pop_last(stack);
    if (
            item->token->type == TOKEN_ID &&
            dollar->token->type == DOLLAR &&
            stack->first == NULL)
    {
        free(item);
        free(dollar);
        return 0;
    }

//    free(item);
//    free(dollar);
    return 1;
}

TOKEN_T* create_dollar_token()
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = DOLLAR;

    return token;
}