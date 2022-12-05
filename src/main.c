#include <stdlib.h>
#include <stdio.h>


#include "dyn_string.h"
#include "lexer.h"
#include "token.h"
#include "syntax_and_semantic_analyzer.h"
#include "symtable.h"

void insert_builtins(htab_t *symtable)
{
    enum T_KEYWORD one_string_param[1];
    one_string_param[0] = KEY_STRING;

    enum T_KEYWORD one_int_param[1];
    one_string_param[0] = KEY_INT;

    enum T_KEYWORD substring_param[3];
    substring_param[0] = KEY_STRING;
    substring_param[1] = KEY_INT;
    substring_param[2] = KEY_INT;

    htab_insert_func(symtable, "reads", KEY_STRING, 0, NULL);
    htab_insert_func(symtable, "readi", KEY_INT, 0, NULL);
    htab_insert_func(symtable, "readf", KEY_FLOAT, 0, NULL);

    // htab_insert_func(symtable, "floatval", KEY_FLOAT, , );
    // htab_insert_func(symtable, "intval", KEY_INT, , );
    // htab_insert_func(symtable, "strval", KEY_STRING, , );

    htab_insert_func(symtable, "strlen", KEY_INT, 1, one_string_param);
    htab_insert_func(symtable, "substring", KEY_STRING, 3, substring_param);

    htab_insert_func(symtable, "ord", KEY_INT, 1, one_string_param);
    htab_insert_func(symtable, "chr", KEY_STRING, 1, one_int_param);
}


//void WriteToken(TOKEN_T* token);

int main() {
    htab_t *symtable = htab_init(20);

    insert_builtins(symtable);

    analyze_token(symtable);
    //nahrazeno analyze token
    //SNS_start();
    //FILE *file = fopen("php_examples/var_assign.php", "r");

//    TOKEN_T * token = NULL;
//
//    do {
//        // TODO redo
//        if (token != NULL) { free(token); token = NULL; }
//
//        token = get_next_token();
////       // analyze_token_key(*token);
//        WriteToken(token);
////
////        // TODO remove
//    } while (token->type != ISEOF);
////
////
////    // TODO remove
//    free(token); token = NULL;

    return 0;
}



