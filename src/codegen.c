#include "codegen.h"

int tmp_var = 0;
int tmp_if = 0;

void print_token_value(TOKEN_T* token, char* frame)
{
    if (token->type == LITERAL)
    {
        if (token->value.type == 0)
        {
            printf("int@%d", token->value.int_val);
        }
        if (token->value.type == 1)
        {
            printf("string@%s", token->value.char_val);
        }
        if (token->value.type == 2)
        {
            printf("float@%f", token->value.double_val);
        }
    }
    else if(token->type == TOKEN_ID)
    {
        printf("%s@%s", frame, token->name);
    }
}

void print_expression(BSTnode* node, TOKEN_T* token, char* operation)
{

    if (node->left->token->type == OPERATOR && node->right->token->type == OPERATOR)
    {
        // TODO: potreba pomocna promenna, pouzit frame?
        printf(">>>>>>CODEGEN>>>>>> %s TF@%s%d, TF@%s%d, TF@%s%d\n", operation, token->name, tmp_var,
               token->name, tmp_var - 1,
               token->name, tmp_var
               );
        printf(">>>>>>CODEGEN>>>>>> MOV LF@%s, TF@%s%d\n", token->name, token->name, tmp_var);
    }
    else if (node->left->token->type == OPERATOR)
    {
//        printf(">>>>>>CODEGEN>>>>>> DEFVAR TF@%s%d\n", token->name, ++tmp_var);
        printf(">>>>>>CODEGEN>>>>>> %s LF@%s, LF@%s, ", operation, token->name, token->name);
        print_token_value(node->right->token, "LF");
        printf("\n");

//        printf(">>>>>>CODEGEN>>>>>> MOV LF@%s, TF@%s%d\n", token->name, token->name, tmp_var);
//        tmp_var--;
    }
    else if (node->right->token->type == OPERATOR)
    {
//        printf(">>>>>>CODEGEN>>>>>> DEFVAR TF@%s%d\n", token->name, ++tmp_var);
        printf(">>>>>>CODEGEN>>>>>> %s LF@%s, LF@%s, ", operation, token->name, token->name);
        print_token_value(node->left->token, "LF");
        printf("\n");

//        printf(">>>>>>CODEGEN>>>>>> MOV LF@%s, TF@%s%d\n", token->name, token->name, tmp_var);
//        tmp_var--;
    }
    else
    {
        printf(">>>>>>CODEGEN>>>>>> DEFVAR TF@%s%d\n", token->name, ++tmp_var);
        printf(">>>>>>CODEGEN>>>>>> %s TF@%s%d, ", operation, token->name, tmp_var);
        print_token_value(node->left->token, "LF");
        printf(", ");
        print_token_value(node->right->token, "LF");
        printf("\n");

        printf(">>>>>>CODEGEN>>>>>> MOV LF@%s, TF@%s%d\n", token->name, token->name, tmp_var);
    }

}


void select_expression(BSTnode* node, TOKEN_T* token)
{
    if (node->token->type != OPERATOR)
    {
        return;
    }

    if (node->token->operators == PLUS)
    {
        print_expression(node, token, "ADD");
    }
    else if (node->token->operators == MULTIPLY)
    {
        print_expression(node, token, "MUL");
    }
    else if (node->token->operators == MINUS)
    {
        print_expression(node, token, "SUB");
    }
    else if (node->token->operators == DIVIDE)
    {
        // if result is integer
        if (node->token->keyword == KEY_INT)
        {
            print_expression(node, token, "IDIV");
        }
        else
        {
            print_expression(node, token, "DIV");
        }
    }
    else if (node->token->operators == LESS)
    {
        print_expression(node, token, "LT");
    }
    else if (node->token->operators == GREATER)
    {
        print_expression(node, token, "GT");
    }
    else if (node->token->operators == EQUALS)
    {
        print_expression(node, token, "EQ");
    }
//    else if (node->token->operators == LESS_EQUAL)
//    {
//        print_expression(node, token, "LT");
//    }
//    else if (node->token->operators == GREATER_EQUAL)
//    {
//        print_expression(node, token, "GT");
//    }
    else if (node->token->operators == CONCAT)
    {
        print_expression(node, token, "CONCAT");
    }
    // TODO: add more operations
}

void iterate_tree(BSTnode* node, TOKEN_T* token)
{
    if (node == NULL) return;
    if (node->left) iterate_tree(node->left, token);
    if (node->right) iterate_tree(node->right, token);

    select_expression(node, token);
//    printf("visited: %s\n", node->token->name);
}


int gen_declare_var(TOKEN_T* token, int scope) {

}



// token = expression
int gen_expression(TOKEN_T* token, BSTnode* node, int scope, bool isDeclaration)
{

    if (token->type == TOKEN_ID && isDeclaration)
    {
        printf(">>>>>>CODEGEN>>>>>> DEFVAR LF@%s\n", token->name);
        if (node->token->type == LITERAL)
        {
            printf(">>>>>>CODEGEN>>>>>> MOV LF@%s, ", token->name);print_token_value(node->token, "LF");printf("\n");
        }
    }

    if (node->left || node->right)
    {
//        BST_print(node, 2);
        tmp_var = 0;
        printf(">>>>>>CODEGEN>>>>>> PUSHFRAME\n");
        iterate_tree(node, token);
        printf(">>>>>>CODEGEN>>>>>> POPFRAME\n");
        tmp_var = 0;
    }

    return 0;
}

void gen_if(BSTnode* node){
    printf("\n\n\nCall of gen_if\n\n\n");
}