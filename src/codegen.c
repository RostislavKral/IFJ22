#include "codegen.h"

int tmp_var = 0;
int tmp_if = 0;

int if_count = 0;   // total if count
int if_stack = 0;   // number of current if dephs

int while_count = 0;   // total while count
int while_stack = 0;   // number of current while dephs

void gen_header()
{
    printf(".IFJcode22\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
}

void gen_write(TOKEN_T* token)
{
    printf("WRITE ");
    print_token_value(token, "LF");
    printf("\n");
}

void gen_func_call(htab_item_t* item, char* name)
{
    // TODO: add builtin functions
    if (strcmp(name, "reads") == 0)
    {

    }
    else if (strcmp(name, "readi") == 0)
    {

    }
    else if (strcmp(name, "reads") == 0)
    {

    }
    else if (strcmp(name, "strlen") == 0)
    {

    }
    else if (strcmp(name, "substring") == 0)
    {

    }
    else if (strcmp(name, "ord") == 0)
    {

    }
    else if (strcmp(name, "chr") == 0)
    {

    }
    printf("\n");
}

// token = expression
int gen_expression(TOKEN_T* token, BSTnode* node, int scope, bool isDeclaration)
{

    if (token->type == TOKEN_ID && isDeclaration)
    {
        printf("DEFVAR LF@%s\n", token->name);
        if (node->token->type == LITERAL)
        {
            printf("MOVE LF@%s ", token->name);print_token_value(node->token, "LF");printf("\n");
        }
    }

    if (node->left || node->right)
    {
//        BST_print(node, 2);
        tmp_var = 0;
        printf("CREATEFRAME\n");
        printf("DEFVAR TF@first_tmp\n");
        printf("DEFVAR TF@second_tmp\n");
        iterate_tree(node, token);
        printf("CREATEFRAME\n");
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

    printf("DEFVAR LF@%s\n", "tmp_if_expr_var");
    printf("MOVE LF@%s int@0\n", "tmp_if_expr_var");
    TOKEN_T * token = malloc(sizeof(TOKEN_T));
    token->name = "tmp_if_expr_var";

    gen_expression(token, node, 1, true); // 1 for local scope ( TODO )

    char * jmpName = get_jmp_name(1); 
    printf("JUMPIFEQ %s LF@%s int@0\n", jmpName, "tmp_if_expr_var");
    free(jmpName);

    free(token);
}

int gen_else(BSTnode* conditionTree) {
    char * jmpName = get_jmp_name(2);
    printf("JUMP %s\n", jmpName);
    free(jmpName);
    jmpName = get_jmp_name(1);
    printf("LABEL %s\n", jmpName);
    free(jmpName);

    return 0;
}

int gen_else_exit(BSTnode* conditionTree) {
    char * jmpName = get_jmp_name(2);
    printf("LABEL %s\n", jmpName);
    if_stack--;
    free(jmpName);

    return 0;    
}




void gen_while(BSTnode* node) {
    if_stack++;
    if_count++;

    printf("DEFVAR TF@%s\n", "tmp_while_expr_var");
    printf("MOVE TF@%s int@0\n", "tmp_while_expr_var");
    TOKEN_T * token = malloc(sizeof(TOKEN_T));
    token->name = "tmp_while_expr_var";

    char * jmpName = get_jmp_name(4); 
    printf("JUMPIFEQ %s TF@%s int@0\n", jmpName, "tmp_if_expr_var");
    free(jmpName);

    free(token);
}


void gen_while_exit(BSTnode* node) {
    char * jmpName = get_jmp_name(3); 
    printf("JUMP %s\n", jmpName);
    free(jmpName);
    
    jmpName = get_jmp_name(4); 
    printf("LABEL %s\n", jmpName);
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
            printf("float@%a", token->value.double_val);
        }
    }
    else if(token->type == TOKEN_ID)
    {
        printf("%s@%s", frame, token->name);
    }
}

//void print_literal(TOKEN_T* token)
//{
//    if (token->value.type == 0)
//    {
//        printf("int@%d", token->value.int_val);
//    }
//    if (token->value.type == 1)
//    {
//        printf("string@%s", token->value.char_val);
//    }
//    if (token->value.type == 2)
//    {
//        printf("float@%a", token->value.double_val);
//    }
//}

char* get_instruction(BSTnode* node)
{
    if (node->token->operators == PLUS)
    {
        return "ADD";
    }
    else if (node->token->operators == MULTIPLY)
    {
        return "MUL";
    }
    else if (node->token->operators == MINUS)
    {
        return "SUB";
    }
    else if (node->token->operators == DIVIDE)
    {
        // if result is integer
        if (node->left->type == KEY_INT && node->right->type == KEY_INT)
        {
//            return "IDIV";
            return "DIV";
        }
        else
        {
            return "DIV";
        }
    }
    else if (node->token->operators == LESS)
    {
        return "LT";
    }
    else if (node->token->operators == GREATER)
    {
        return "GT";
    }
    else if (node->token->operators == EQUALS)
    {
        return "EQ";
    }
        // TODO: LESS_EQUAL = !GREATHER and GREATER_EQUAL = !LESS (use NOT instruction)
//    else if (node->token->operators == LESS_EQUAL)
//    {
//        return "LT";
//    }
//    else if (node->token->operators == GREATER_EQUAL)
//    {
//        return "GT";
//    }
    else if (node->token->operators == CONCAT)
    {
        return "CONCAT";
    }
    // TODO: add more operations
}

void select_expression(BSTnode* node, TOKEN_T* token)
{
    if (node->token->type != OPERATOR)
    {
        return;
    }

    char* instruction = get_instruction(node);

    switch (node->token->operators) {
        case MULTIPLY:
        case DIVIDE:
        case PLUS:
        case MINUS:
            if (node->left->type == KEY_INT && node->right->type == KEY_INT){}
            else if (node->left->type == KEY_FLOAT && node->right->type == KEY_FLOAT) {}
            else if (node->right->type == KEY_FLOAT)
            {
                printf("MOVE TF@first_tmp ");print_token_value(node->left->token, "LF");printf("\n");
                printf("MOVE TF@second_tmp ");print_token_value(node->right->token, "LF");printf("\n");
                printf("INT2FLOAT TF@first_tmp TF@first_tmp\n");
            }
            else if (node->left->type == KEY_FLOAT)
            {
                printf("MOVE TF@first_tmp ");print_token_value(node->left->token, "LF");printf("\n");
                printf("MOVE TF@second_tmp ");print_token_value(node->right->token, "LF");printf("\n");
                printf("INT2FLOAT TF@second_tmp TF@second_tmp\n");
            }
            break;
        case CONCAT:
            printf("MOVE TF@first_tmp ");print_token_value(node->left->token, "LF");printf("\n");
            printf("MOVE TF@second_tmp ");print_token_value(node->right->token, "LF");printf("\n");
            break;
        case LESS:
        case GREATER:
        case LESS_EQUAL:
        case GREATER_EQUAL:
            break;
        case TYPE_EQUALS:
        case TYPE_NOT_EQUALS:
            break;
        default:
            break;
    }

    print_expression(node, token, instruction);
}

void print_expression(BSTnode* node, TOKEN_T* token, char* operation)
{
    // TODO: Define temporary variables and MOVE values to them and execute type conversion

    if (node->left->token->type == OPERATOR && node->right->token->type == OPERATOR)
    {
        printf("%s TF@%s%d TF@%s%d TF@%s%d\n", operation, token->name, tmp_var,
               token->name, tmp_var - 1,
               token->name, tmp_var
        );
        printf("MOVE LF@%s TF@%s%d\n", token->name, token->name, tmp_var);
    }
    else if (node->left->token->type == OPERATOR)
    {
        printf("%s LF@%s LF@%s ", operation, token->name, token->name);
        print_token_value(node->right->token, "LF");
        printf("\n");
    }
    else if (node->right->token->type == OPERATOR)
    {
        printf("%s LF@%s LF@%s, ", operation, token->name, token->name);
        print_token_value(node->left->token, "LF");
        printf("\n");
    }
    else
    {
        printf("DEFVAR TF@%s%d\n", token->name, ++tmp_var);
        printf("%s TF@%s%d TF@first_tmp TF@second_tmp", operation, token->name, tmp_var);
//        print_token_value(node->left->token, "LF");
//        printf(" ");
//        print_token_value(node->right->token, "LF");
        printf("\n");

        printf("MOVE LF@%s TF@%s%d\n", token->name, token->name, tmp_var);
    }

}