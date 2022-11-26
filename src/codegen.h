#ifndef CODEGEN_H
#define CODEGEN_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "syntax_and_semantic_analyzer.h"



int gen_code(RULE_T * rule);


#endif