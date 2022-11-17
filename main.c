#include <stdio.h>
#include "utilities/dyn_string.h"
#include "lexer.h"

int main() {

    FILE *file = fopen("file.php", "r");

    struct T_TOKEN token;

    while(token.type != ISEOF)
    {
        token = get_next_token(file);

        printf("%u   ", token.type);
        printf("%s   \n", token.name);

    }


    return 0;
}
