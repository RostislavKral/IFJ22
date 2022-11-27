//
// Created by rostik on 10/20/22.
//

#ifndef IFJ2_DYN_STRING_H
#define IFJ2_DYN_STRING_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#endif //IFJ2_DYN_STRING_H





struct T_DYN_STRING {
    size_t max_len;
    int inc;
    char *str;
};
typedef struct T_DYN_STRING dyn_string;


int str_conc(dyn_string *str, char *str_to_add);

/*
int main() {
    dyn_string str;
    str.inc = 10;
    str.max_len = 10;

    str.str = malloc (str.max_len * sizeof(char));


    str_conc(&str, "HELLO");
    str_conc(&str, "HELLO");
    str_conc(&str, "HELLO");
    str_conc(&str, "HELLO");
    printf("%s\n", str.str);

    free(str.str);
    return 0;
}*/