#include <stdlib.h>
#include <stdio.h>


#include "dyn_string.h"
#include "lexer.h"
#include "token.h"
#include "syntax_and_semantic_analyzer.h"
#include "symtable.h"



//void WriteToken(TOKEN_T* token);

int main() {
    //htab_t *symtable = htab_init(20);
    //analyze_token(symtable);
    //nahrazeno analyze token
    //SNS_start();
    //FILE *file = fopen("php_examples/var_assign.php", "r");

    TOKEN_T * token = NULL;

    do {
        // TODO redo
        if (token != NULL) { free(token); token = NULL; }

        token = get_next_token();
//       // analyze_token_key(*token);
        WriteToken(token);
//
//        // TODO remove
    } while (token->type != ISEOF);
//
//
//    // TODO remove
    free(token); token = NULL;

    return 0;
}



