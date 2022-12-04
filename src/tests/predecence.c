#include "../expression_stack.h"
#include "../expression_stack.c"
#include "../expressions.h"
#include "../expressions.c"
#include "../token.h"


void add_var(DLList* stack)
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = TOKEN_ID;
    token->name = "a";
    DLL_insert_last(stack, token);
}

void add_op(DLList* stack, int a)
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = OPERATOR;
    token->name = NULL;
    token->operators = PLUS;
    DLL_insert_last(stack, token);
}

void add_type(DLList* stack, int type)
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = type;
    token->name = NULL;
    DLL_insert_last(stack, token);
}

int get_result(DLList* stack)
{
    int result = analyze_precedence(stack);
    if (result == 0)
    {
        printf("Success");
        return 0;
    }
    else if(result == 1)
    {
        printf("Syntax error, expression in not ended\n");
    }
    else if(result == -1)
    {
        printf("Syntax error, inside expression\n");
    }
    return 1;
}

int test_expr1()
{
    printf("\n___________________________\n\n");
    printf("Test expression a + b + c");
    printf("\n___________________________\n");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack);

    // a +
    add_op(stack, PLUS);

    // a + b
    add_var(stack);

    // a + b +
    add_op(stack, PLUS);

    // a + b + c
    add_var(stack);

    return get_result(stack);
}

int test_expr2()
{
    printf("\n___________________________\n");
    printf("Test expression a + b * c");
    printf("\n___________________________\n");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack);

    // a +
    add_op(stack, PLUS);

    // a + b
    add_var(stack);

    // a + b *
    add_op(stack, MULTIPLY);

    // a + b * c

    add_var(stack);

    return get_result(stack);
}


int test_expr3()
{
    printf("\n___________________________\n");
    printf("Test expression a + (b * c)");
    printf("\n___________________________\n");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack);

    // a +
    add_op(stack, PLUS);

    // a + (
    add_type(stack, LPAR);

    // a + (b
    add_var(stack);

    // a + (b +
    add_op(stack, MULTIPLY);

    // a + (b * c
    add_var(stack);

    // a + (b * c)
    add_type(stack, RPAR);

    return get_result(stack);
}

int test_expr4()
{
    printf("\n___________________________\n");
    printf("Test expression a + (a * (a * a))");
    printf("\n___________________________\n");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack);

    // a +
    add_op(stack, PLUS);

    // a + (
    add_type(stack, LPAR);
    // a + (a
    add_var(stack);
    // a + (a *
    add_op(stack, MULTIPLY);
    // a + (a * (
    add_type(stack, LPAR);
    // a + (a * (a
    add_var(stack);
    // a + (a * (a +
    add_op(stack, PLUS);
    // a + (a * (a + a
    add_var(stack);
    // a + (a * (a + a)
    add_type(stack, RPAR);
    // a + (a * (a + a))
    add_type(stack, RPAR);

    return get_result(stack);
}

int test_expr5()
{
    printf("\n___________________________\n");
    printf("Test expression a < a");
    printf("\n___________________________\n");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack);

    // a <
    add_op(stack, LESS);

    // a < a
    add_var(stack);

    return get_result(stack);
}

int test_expr6()
{
    printf("\n___________________________\n");
    printf("Test expression a a");
    printf("\n___________________________\n");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack);

    // a < a
    add_var(stack);

    return get_result(stack);
}

int main()
{
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int e = 0;

    a = test_expr1();
    b = test_expr2();
    c = test_expr3();
    d = test_expr4();
    e = test_expr5();
    test_expr6();
//    printf("%d%d%d%d%d", a,b,c,d,e);
}