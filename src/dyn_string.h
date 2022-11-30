/**
 * @file dyn_string.h
 * @author Nikita Sniehovskyi (xsnieh00)
 * @brief Header file for dynamic string
 */

#ifndef IFJ2_DYN_STRING_H
#define IFJ2_DYN_STRING_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct DYN_STRING {
    size_t len;
    char *str;
};
typedef struct DYN_STRING DYN_STRING_T;

int str_conc(DYN_STRING_T *str, char *str_to_add);
int str_init(DYN_STRING_T *str);
int str_destroy(DYN_STRING_T *str);

#endif