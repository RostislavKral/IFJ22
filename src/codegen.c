#include "codegen.h"

int tmp_var = 0;
int tmp_if = 0;

int if_count = 0;   // total if count
int if_stack = 0;   // number of current if dephs

int while_count = 0;   // total while count
int while_stack = 0;   // number of current while dephs



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
        printf(">>>>>>CODEGEN>>>>>> CREATEFRAME\n");
        iterate_tree(node, token);
        printf(">>>>>>CODEGEN>>>>>> CREATEFRAME\n");
        tmp_var = 0;
    }

    return 0;
}





int get_num_len(int num) {
    if (num == 0) { return 0; }
    return get_num_len(num / 10) + 1;
}

char * get_jmp_name(int blablabla) {
    char * name = malloc(sizeof(char) * (get_num_len(if_count) + get_num_len(if_stack) + 5));
    switch (blablabla) {
    case 0: // if
        sprintf(name, "if_%d_%d", if_count, if_stack);
        break;
    case 1:// else
        sprintf(name, "el_%d_%d", if_count, if_stack);
        break;
    case 2: // else exit
        sprintf(name, "ex_%d_%d", if_count, if_stack);
    case 3: // while start
        sprintf(name, "wh_%d_%d", while_count, while_stack);
    case 4: // while end
        sprintf(name, "we_%d_%d", while_count, while_stack);
    default:
        break;
    }

    return name;
}

int gen_if(BSTnode* node){
    if_stack++;
    if_count++;

    printf(">>>>>>CODEGEN>>>>>> DEFVAR TF@%s\n", "tmp_if_expr_var");
    printf(">>>>>>CODEGEN>>>>>> MOV TF@%s 0\n", "tmp_if_expr_var");
    TOKEN_T * token = malloc(sizeof(TOKEN_T));
    token->name = "tmp_if_expr_var";

    gen_expression(token, node, 1, false); // 1 for local scope ( TODO )

    char * jmpName = get_jmp_name(1); 
    printf(">>>>>>CODEGEN>>>>>> JUMPIFEQ %s TF@%s 0\n", jmpName, "tmp_if_expr_var");
    free(jmpName);

    free(token);
}

int gen_else() {
    char * jmpName = get_jmp_name(2);
    printf(">>>>>>CODEGEN>>>>>> JUMP %s\n", jmpName);
    free(jmpName);
    jmpName = get_jmp_name(1);
    printf(">>>>>>CODEGEN>>>>>> LABEL %s\n", jmpName);
    free(jmpName);

    return 0;
}

int gen_else_exit() {
    char * jmpName = get_jmp_name(2);
    printf(">>>>>>CODEGEN>>>>>> LABEL %s\n", jmpName);
    if_stack--;
    free(jmpName);

    return 0;    
}

void gen_while(BSTnode* node) {
    if_stack++;
    if_count++;

    printf(">>>>>>CODEGEN>>>>>> DEFVAR TF@%s\n", "tmp_while_expr_var");
    printf(">>>>>>CODEGEN>>>>>> MOV TF@%s 0\n", "tmp_while_expr_var");
    TOKEN_T * token = malloc(sizeof(TOKEN_T));
    token->name = "tmp_while_expr_var";

    char * jmpName = get_jmp_name(4); 
    printf(">>>>>>CODEGEN>>>>>> JUMPIFEQ %s TF@%s 0\n", jmpName, "tmp_if_expr_var");
    free(jmpName);

    free(token);
}


void gen_while_exit(BSTnode* node) {
    char * jmpName = get_jmp_name(3); 
    printf(">>>>>>CODEGEN>>>>>> JUMP %s\n", jmpName);
    free(jmpName);
    
    jmpName = get_jmp_name(4); 
    printf(">>>>>>CODEGEN>>>>>> LABEL %s\n", jmpName);
    free(jmpName);

    if_stack--;
}


void gen_function()
{
    printf("\n\n\nCall of gen function\n\n\n");
}




void iterate_tree(BSTnode* node, TOKEN_T* token)
{
    if (node == NULL) return;
    if (node->left) iterate_tree(node->left, token);
    if (node->right) iterate_tree(node->right, token);

    select_expression(node, token);
//    printf("visited: %s\n", node->token->name);
}

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
        if (node->left->type == KEY_INT && node->right->type == KEY_INT)
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
    // TODO: LESS_EQUAL = !GREATHER and GREATER_EQUAL = !LESS (use NOT instruction)
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

void print_expression(BSTnode* node, TOKEN_T* token, char* operation)
{
    // TODO: Define temporary variables and MOVE values to them and execute type conversion

    if (node->left->token->type == OPERATOR && node->right->token->type == OPERATOR)
    {
        printf(">>>>>>CODEGEN>>>>>> %s TF@%s%d, TF@%s%d, TF@%s%d\n", operation, token->name, tmp_var,
               token->name, tmp_var - 1,
               token->name, tmp_var
        );
        printf(">>>>>>CODEGEN>>>>>> MOV LF@%s, TF@%s%d\n", token->name, token->name, tmp_var);
    }
    else if (node->left->token->type == OPERATOR)
    {
        printf(">>>>>>CODEGEN>>>>>> %s LF@%s, LF@%s, ", operation, token->name, token->name);
        print_token_value(node->right->token, "LF");
        printf("\n");
    }
    else if (node->right->token->type == OPERATOR)
    {
        printf(">>>>>>CODEGEN>>>>>> %s LF@%s, LF@%s, ", operation, token->name, token->name);
        print_token_value(node->left->token, "LF");
        printf("\n");
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