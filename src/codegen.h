#ifndef CODEGEN_H
#define CODEGEN_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "syntax_and_semantic_analyzer.h"
#include "token.h"



// int gen_code(RULE_T * rule);
int gen_expression(TOKEN_T* token, BSTnode* node, int scope, bool isDeclaration);
void gen_if(BSTnode* conditionTree);
int gen_declare_var(TOKEN_T* token, int scope);

#endif