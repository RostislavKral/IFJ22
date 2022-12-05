/**
 * @file tests/predecence.c
 * @author Hoang Nam Nguyen (xnguye22)
 * @brief Tests for precedence analysis (expressions)
 */

#include "../expression_stack.h"
#include "../expression_stack.c"
#include "../expressions.h"
#include "../expressions.c"
#include "../token.c"


void add_var(DLList* stack, char* name)
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = TOKEN_ID;
    token->name = name;
    DLL_insert_last(stack, token);
}

void add_int(DLList* stack, int value)
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = LITERAL;
    token->value.type = 0;
    token->value.int_val = value;
    DLL_insert_last(stack, token);
}
void add_string(DLList* stack, char* string)
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = LITERAL;
    token->value.type = 1;
    token->value.char_val = string;
    DLL_insert_last(stack, token);
}
void add_double(DLList* stack, double value)
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = LITERAL;
    token->value.type = 2;
    token->value.double_val = value;
    DLL_insert_last(stack, token);
}

void add_op(DLList* stack, int a)
{
    TOKEN_T* token = malloc(sizeof (TOKEN_T));
    token->type = OPERATOR;
    token->name = NULL;
    token->operators = a;
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
    BSTnode* result = analyze_precedence(stack);
//    printf("tree: \n");BST_print(result, 0);
    if (result != NULL)
    {
        printf("\nSuccess");
        return 0;
    }
//    else if(result == 1)
//    {
//        printf("Syntax error, expression in not ended\n");
//    }
//    else if(result == -1)
//    {
//        printf("Syntax error, inside expression\n");
//    }
    else
    {
        printf("\n\n>>> Syntax error");
    }
    return 1;
}

int test_expr1()
{
    printf("\n___________________________\n\n");
    printf("Test expression a + b + c");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack, "a");

    // a +
    add_op(stack, PLUS);

    // a + b
    add_var(stack, "b");

    // a + b +
    add_op(stack, PLUS);

    // a + b + c
    add_var(stack, "c");

    return get_result(stack);
}

int test_expr2()
{
    printf("\n___________________________\n");
    printf("Test expression a + b * c");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack, "a");

    // a +
    add_op(stack, PLUS);

    // a + b
    add_var(stack, "b");

    // a + b *
    add_op(stack, MULTIPLY);

    // a + b * c

    add_var(stack, "c");

    return get_result(stack);
}


int test_expr3()
{
    printf("\n___________________________\n");
    printf("Test expression a + (b * c)");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack, "a");

    // a +
    add_op(stack, PLUS);

    // a + (
    add_type(stack, LPAR);

    // a + (b
    add_var(stack, "b");

    // a + (b +
    add_op(stack, MULTIPLY);

    // a + (b * c
    add_var(stack, "c");

    // a + (b * c)
    add_type(stack, RPAR);

    return get_result(stack);
}

int test_expr4()
{
    printf("\n___________________________\n");
    printf("Test expression a + (b * (c * d))");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack, "a");

    // a +
    add_op(stack, PLUS);

    // a + (
    add_type(stack, LPAR);
    // a + (b
    add_var(stack, "b");
    // a + (b *
    add_op(stack, MULTIPLY);
    // a + (b * (
    add_type(stack, LPAR);
    // a + (b * (c
    add_var(stack, "c");
    // a + (b * (c +
    add_op(stack, PLUS);
    // a + (b * (c + d
    add_var(stack, "d");
    // a + (b * (c + d)
    add_type(stack, RPAR);
    // a + (b * (c + d))
    add_type(stack, RPAR);

    return get_result(stack);
}

int test_expr5()
{
    printf("\n___________________________\n");
    printf("Test expression a < b");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack, "a");

    // a <
    add_op(stack, LESS);

    // a < b
    add_var(stack, "b");

    return get_result(stack);
}

int test_expr6()
{
    printf("\n___________________________\n");
    printf("Test expression a b");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    // a
    add_var(stack, "a");

    // a b
    add_var(stack, "b");

    return get_result(stack);
}

int test_expr7()
{
    printf("\n___________________________\n");
    printf("Test expression a + b <= c / d");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_var(stack, "a");
    add_op(stack, PLUS);
    add_var(stack, "b");
    add_op(stack, LESS_EQUAL);
    add_var(stack, "c");
    add_op(stack, DIVIDE);
    add_var(stack, "d");

    return get_result(stack);
}

int test_expr8()
{
    printf("\n___________________________\n");
    printf("Test expression a + +");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_var(stack, "a");
    add_op(stack, PLUS);
    add_op(stack, PLUS);

    return get_result(stack);
}

int test_expr9()
{
    printf("\n___________________________\n");
    printf("Test expression (a + c < d) < a");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_type(stack, LPAR);
    add_var(stack, "a");
    add_op(stack, PLUS);
    add_var(stack, "c");
    add_op(stack, LESS);
    add_var(stack, "d");
    add_type(stack, RPAR);
    add_op(stack, LESS);
    add_var(stack, "a");

    return get_result(stack);
}

int test_expr10()
{
    printf("\n___________________________\n");
    printf("Test expression 5 + 10");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_int(stack, 5);
    add_op(stack, PLUS);
    add_int(stack, 10);

    return get_result(stack);
}

int test_expr11()
{
    printf("\n___________________________\n");
    printf("Test expression 5 + \"ahoj\"");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_int(stack, 5);
    add_op(stack, PLUS);
    add_string(stack, "ahoj");

    return get_result(stack);
}

int test_expr12()
{
    printf("\n___________________________\n");
    printf("Test expression \"c\" . \"ahoj\"");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_string(stack, "c");
    add_op(stack, CONCAT);
    add_string(stack, "ahoj");

    return get_result(stack);
}

int test_expr13()
{
    printf("\n___________________________\n");
    printf("Test expression \"c\" + \"ahoj\"");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_string(stack, "c");
    add_op(stack, PLUS);
    add_string(stack, "ahoj");

    return get_result(stack);
}

int test_expr14()
{
    printf("\n___________________________\n");
    printf("Test expression 5 . 6");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_int(stack, 5);
    add_op(stack, CONCAT);
    add_int(stack, 6);

    return get_result(stack);
}

int test_expr15()
{
    printf("\n___________________________\n");
    printf("Test expression 5.1 + 6.4");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_double(stack, 5.1);
    add_op(stack, PLUS);
    add_double(stack, 6.4);

    return get_result(stack);
}

int test_expr16()
{
    printf("\n___________________________\n");
    printf("Test expression 5.1 . 6.4");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_double(stack, 5.1);
    add_op(stack, CONCAT);
    add_double(stack, 6.4);

    return get_result(stack);
}

int test_expr17()
{
    printf("\n___________________________\n");
    printf("Test expression 5.1 !== 6.4");
    DLList* stack = malloc(sizeof (DLList*));
    DLL_init(stack);

    add_double(stack, 5.1);
    add_op(stack, TYPE_NOT_EQUALS);
    add_double(stack, 6.4);

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
    test_expr7();
    test_expr8();
    test_expr9();
    test_expr10();
    test_expr11();
    test_expr12();
    test_expr13();
    test_expr14();
    test_expr15();
    test_expr16();
    test_expr17();
//    printf("%d%d%d%d%d", a,b,c,d,e);
}