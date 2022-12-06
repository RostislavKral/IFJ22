#include "codegen.h"

void print_token_value(TOKEN_T* token)
{
    if (token->type == LITERAL)
    {
        if (token->value.type == 0)
        {
            printf("%d", token->value.int_val);
        }
        if (token->value.type == 1)
        {
            printf("%s", token->value.char_val);
        }
        if (token->value.type == 2)
        {
            printf("%f", token->value.double_val);
        }
    }
    else if(token->type == TOKEN_ID)
    {
        printf("%s", token->name);
    }
}

void print_expression(BSTnode* node, TOKEN_T* token, char* operation)
{
    if (node->left->token->type == OPERATOR)
    {
        printf(">>>>>>CODEGEN>>>>>> %s %s, %s, ", operation, token->name, token->name);
        print_token_value(node->right->token);
        printf("\n");
    }
    else if (node->right->token->type == OPERATOR)
    {
        printf(">>>>>>CODEGEN>>>>>> %s %s, %s, ", operation, token->name, token->name);
        print_token_value(node->left->token);
        printf("\n");
    }
    else if (node->left->token->type == OPERATOR && node->right->token->type == OPERATOR)
    {
        // TODO: potreba pomocna promenna
//        printf(">>>>>>CODEGEN>>>>>> %s %s, %s, %s\n", operation, token->name, token->name, node->right->token->name);
    }
    else
    {
        printf(">>>>>>CODEGEN>>>>>> %s %s, ", operation, token->name);
        print_token_value(node->left->token);
        printf(", ");
        print_token_value(node->right->token);
        printf("\n");
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
    }
//    // printf(">%d\n", node->type);
//    printf(">>>>>>CODEGEN>>>>>> DEFVAR LF@%s\n", token->name);
//    // BST_print(node, 2);
//    if (node->token->type == LITERAL) {
//
//        switch (node->token->keyword) {
//        case KEY_INT:
//            printf(">>>>>>CODEGEN>>>>>> MOVE LF@%s int@%d\n", token->name, node->token->value.int_val);
//            break;
//
//        default:
//            break;
//        }
//
//    } else {
//
//    }
    BST_print(node, 2);
    iterate_tree(node, token);

    return 0;
}

void gen_if(BSTnode* conditionTree){
    printf("\n\n\nCall of gen_if\n\n\n");
}