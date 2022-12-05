/**
 * @file sym_table_htab.h
 * @author Nikita Sniehovskyi
 * @brief Table of symbols based on hash table
 * @version 0.1
 * @date 2022-10-22
 */


#ifndef __HTAB_H__
#define __HTAB_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h> 

#include "token.h"



/**
 * If less than MIN_SPACE_TAKEN * 100% of table is filled, it will be automaticaly resized 
 */
#define MIN_SPACE_TAKEN 0.25

/**
 * If more than MAX_SPACE_TAKEN * 100% of table is filled, it will be automaticaly resized
 */
#define MAX_SPACE_TAKEN 0.75


/**
 * @brief Describes type of an item
 * 
 */
typedef enum {
    FUNC,
    VAR,
} htab_value_type;

/**
 * @brief Container for variable's value
 * 
 */
typedef union {
    int int_value;
    char *str_value;
    double float_value;
} htab_value;


/**
 * @brief Data container of the element
 * 
 * Fields are filled based on the value type:
 *  - FUNC: 
 *      data_type       -- [0]: return value, [1+] type of return value
 *      params_count    -- number function params
 *      value           -- useless
 * 
 *  - VAR: 
 *      data_type       -- type of the variable
 *      value           -- value of the variable
 *      params_count    -- useless
 */
typedef struct htab_data {
    htab_value_type type;

    enum T_KEYWORD * data_type;
    int params_count;
    htab_value value;
} htab_data_t;


typedef const char * htab_key_t;
typedef struct htab_item htab_item_t;

typedef struct htab_item {
    htab_key_t    key;
    int scope;

    htab_data_t data;

    htab_item_t * next;
} htab_item_t; 

/**
 * @brief Pointer to the first element in the line
 * 
 */
typedef struct htab_link {
    htab_item_t * item;
} htab_link_t;

struct htab {
    size_t size;
    size_t arr_size;
    htab_link_t ** arr_ptr;
};
typedef struct htab htab_t;

/**
 * @brief 
 * @param str 
 * @return size_t 
 */
size_t htab_hash_function(htab_key_t str);

/**
 * @brief Inicialization of a table
 * 
 * @param n Starting number of elememnts
 * @return htab_t * 
 */
htab_t * htab_init(size_t n);

/**
 * @brief Table destructor
 * 
 * @param t hash table 
 */
void htab_free(htab_t * t);

/**
 * @brief Inserts variable into table
 * 
 * @param t hash table
 * @param name name of the variable
 * @param scope level of scope of the variable
 * @param type type of the variable
 * @param value value of the var
 * @return true if value was added
 * @return false --
 * 
 * As a value you need to send a structure. Example:
 * 
 * htab_value value;
 * value.int_value = 10;
 * 
 * ... and send it into a function
 */
bool htab_insert_var(htab_t * t, char * name, int scope, enum T_KEYWORD type, htab_value value);

/**
 * @brief TODO
 * 
 * @param t 
 * @param name 
 * @param scope 
 * @param type 
 * @param value 
 * @return true 
 * @return false 
 */
bool htab_update_var(htab_t *t, char *name, int scope, enum T_KEYWORD type, htab_value value);

/**
 * @brief Searches for a variable in htab
 * 
 * @param t hash table
 * @param key name of the variable
 * @param scope level of scope of the variable
 * @return htab_item_t* pointer to the variable
 */
htab_item_t * htab_find_var(htab_t * t, char * key, int scope);

/**
 * @brief 
 * 
 * @param t hash table
 * @param name name of the function
 * @param ret_val_type type of return value
 * @param params_count number of parameters
 * @param type array of parameters types
 * @return true function was added
 * @return false function already exists
 * 
 * enum T_KEYWORD tarr[3] = { KEY_FLOAT, KEY_FLOAT, KEY_FLOAT };
 */
bool htab_insert_func(htab_t * t, char * name, enum T_KEYWORD ret_val_type, int params_count, enum T_KEYWORD * type);

/**
 * @brief Searches for a function in htab
 * 
 * @param t hash table
 * @param key name of the function
 * @return htab_item_t* pointer to the function
 */
htab_item_t * htab_find_func(htab_t * t, char * key);

/**
 * @brief Revomes all variables from a certain scope
 * 
 * @param t hash table
 * @param scope scope level
 * @return true 
 * @return false 
 */
bool htab_remove_scope(htab_t * t, int scope);

/**
 * @brief Returns total number of items
 * 
 * @param t hash table 
 * @return size_t 
 */
size_t htab_items_count(const htab_t * t);

/**
 * @brief Returns size of a table
 * 
 * @param t hash table 
 * @return size_t 
 */
size_t htab_size(const htab_t * t);

/**
 * @brief Increases / decreases size of t
 * 
 * @param t hash table 
 * @param n 
 * @return void* 
 */
void htab_resize(htab_t *t, size_t n);

void htab_debug_print(htab_t *t);


#endif