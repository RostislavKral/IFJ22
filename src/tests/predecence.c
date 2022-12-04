#include "../expression_stack.h"
#include "../expression_stack.c"
#include "../expressions.h"
#include "../expressions.c"
#include "../token.h"

int main()
{
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = TOKEN_ID;
    token->name = "a";
    DLL_insert_last(stack, token);

    // a +
    token = malloc(sizeof (TOKEN_T));
    token->type = OPERATOR;
    token->name = NULL;
    token->operators = PLUS;
    DLL_insert_last(stack, token);

    // a + b
    token = malloc(sizeof (TOKEN_T));
    token->type = TOKEN_ID;
    token->name = "b";
    DLL_insert_last(stack, token);

    // a + b +
    token = malloc(sizeof (TOKEN_T));
    token->type = OPERATOR;
    token->name = NULL;
    token->operators = PLUS;
    DLL_insert_last(stack, token);

    // a + b + c
    token = malloc(sizeof (TOKEN_T));
    token->type = TOKEN_ID;
    token->name = "c";
    DLL_insert_last(stack, token);

    int result = analyze_precedence(stack);
    if (result == 0)
    {
        printf("Success\n");
    }
    else if(result == 1)
    {
        printf("Syntax error, expression in not ended\n");
    }
    else if(result == -1)
    {
        printf("Syntax error, inside expression\n");
    }
}