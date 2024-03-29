#ifndef CODEGEN_H
#define CODEGEN_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "syntax_and_semantic_analyzer.h"
#include "token.h"



// int gen_code(RULE_T * rule);
void gen_header();
int gen_expression(TOKEN_T* token, BSTnode* node, int scope, bool isDeclaration);
int gen_if(BSTnode* conditionTree);
int gen_else();
int gen_else_exit();
void gen_while(BSTnode* node);
void gen_while_exit();
int gen_declare_var(TOKEN_T* token, int scope);
void gen_write(TOKEN_T* token);

void print_literal(TOKEN_T* token);
void print_token_value(TOKEN_T* token, char* frame);
void print_expression(BSTnode* node, TOKEN_T* token, char* operation);
void iterate_tree(BSTnode* node, TOKEN_T* token);
void select_expression(BSTnode* node, TOKEN_T* token);
void gen_func_call(htab_item_t* item, char* name);
#endif