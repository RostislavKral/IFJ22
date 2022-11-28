#include <stdlib.h>
#include <stdio.h>


#include "dyn_string.h"
#include "lexer.h"
#include "token.h"
#include "syntax_and_semantic_analyzer.h"


#define N


int main() {

    #ifdef N
        #ifdef NONDEBUG
            SNS_start("php_examples/var_assign.php");
        #else
            SNS_start("../php_examples/var_assign.php");
        #endif
    #else
        SNS_start("php_examples/var_assign.php");
    #endif

    return 0;
}


