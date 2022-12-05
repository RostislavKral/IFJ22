#include "codegen.h"



int gen_declare_var(TOKEN_T* token, int scope) {
    
}



// token = expression
int gen_expression(TOKEN_T* token, BSTnode* node)
{
    // printf(">%d\n", node->type);
    printf(">>>>>>CODEGEN>>>>>> DEFVAR LF@%s\n", token->name);
    // BST_print(node, 2);
    if (node->token->type == LITERAL) {

        switch (node->token->keyword) {
        case KEY_INT:
            printf(">>>>>>CODEGEN>>>>>> MOVE LF@%s int@%d\n", token->name, node->token->value.int_val);
            break;
        
        default:
            break;
        }

    } else {

    }

    return 0;
}

void gen_if(BSTnode* conditionTree){
    printf("\n\n\nCall of gen_if\n\n\n");
}