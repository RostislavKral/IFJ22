/**
 * @file dyn_string.c
 * @author Nikita Sniehovskyi (xsnieh00)
 * @brief Implementation of dynamic string
 */

#include "dyn_string.h"


int str_destroy(DYN_STRING_T *str) {
    free(str);
}


int str_init(DYN_STRING_T *str) {
    str->len = 0;
    str->str = NULL;
}


int str_conc(DYN_STRING_T *str, char *str_to_add) {
    char *new_str = malloc((str->len + strlen(str_to_add) + 1) * sizeof(char));
    
    if (str->str != NULL) {
        strcpy(new_str, str->str);
        strcat(new_str, str_to_add);

        free(str->str);
        str->str = new_str;
    } else {
        strcpy(new_str, str_to_add);
        str->str = new_str;
    }    

    str->len = strlen(str->str);

    return 0;
}