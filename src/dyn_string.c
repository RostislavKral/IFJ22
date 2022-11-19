//
// Created by rostik on 10/20/22.
//

#include "dyn_string.h"


int str_conc(dyn_string *str, char *str_to_add) {
    if (strlen(str_to_add) + strlen(str->str) >= str->max_len) {
        char *new_str = malloc((str->max_len + str->inc + strlen(str_to_add)) * sizeof(char));
        strcpy(new_str, str->str);
        strcat(new_str, str_to_add);
        free(str->str);
        str->str = new_str;
    } else {
        strcat(str->str, str_to_add);
    }

    return 0;
}