#include <stdlib.h>
#include <stdio.h>


#include "dyn_string.h"
#include "lexer.h"
#include "token.h"
#include "syntax_and_semantic_analyzer.h"



void WriteToken(TOKEN_T* token);

int main() {

    analyze_token();
    //nahrazeno analyze token
    //SNS_start();
    //FILE *file = fopen("php_examples/var_assign.php", "r");

    /*TOKEN_T * token = NULL;

    do {
        // TODO redo
        if (token != NULL) { free(token); token = NULL; }

        token = get_next_token();
       // analyze_token_key(*token);
        WriteToken(token);

        // TODO remove
    } while (token->type != ISEOF);


    // TODO remove
    free(token); token = NULL;
*/
    return 0;
}






/**
 * @brief Debug function. Writes token
 *
 * @param token
 */
void WriteToken(TOKEN_T * token) {
    switch (token->type) {
        case KEYWORD:
            printf("KEYWORD: %s\n", token->name);
            break;

        case TOKEN_ID:
            printf("TOKEN_ID: name: %s\n", token->name);
            break;

        case FUNC_ID:
            printf("FUNC_ID: name: %s\n", token->name);
            break;

        case LITERAL:
            switch (token->value.type) {
                case 0:
                    printf("LITERAL: INT: value: %d\n", token->value.int_val);
                    break;

                case 1:
                    printf("LITERAL: STRING: value: %s\n", token->value.char_val);
                    break;

                case 2:
                    printf("LITERAL: DOUBLE value: %f\n", token->value.double_val);
                    break;

                default:
                    printf("LITERAL: UNKNOWN TYPE: %d\n", token->value.type);
                    break;
            }
            break;

        case ASSIGN:
            printf("ASSIGN: =\n");
            break;

        case LPAR:
            printf("LPAR: (\n");
            break;

        case RPAR:
            printf("RPAR: )\n");
            break;

        case LBRACE:
            printf("LBRACE: { \n");
            break;
        case RBRACE:
            printf("RBRACE: } \n");
            break;

        case OPERATOR:
            switch (token->operators) {
                case EQUALS:
                    printf("OPERATOR: ==\n");
                    break;

                case PLUS:
                    printf("OPERATOR: +\n");
                    break;

                case MINUS:
                    printf("OPERATOR: -\n");
                    break;

                default:
                    printf("OPERATOR: unknown\n");
                    break;
            }
            break;

        case ISEOF:
            printf("ISEOF\n");
            break;

        default:
            printf("UNKNOWN: type: %d, name: %s, value type: %d\n", token->type, token->name, token->value.type);
            break;
    }
}
