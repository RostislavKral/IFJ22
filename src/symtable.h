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


/**
 * If less than MIN_SPACE_TAKEN * 100% of table is filled, it will be automaticaly resized 
 */
#define MIN_SPACE_TAKEN 0.25

/**
 * If more than MAX_SPACE_TAKEN * 100% of table is filled, it will be automaticaly resized
 */
#define MAX_SPACE_TAKEN 0.75


// Key type
typedef const char * htab_key_t;


typedef enum {
    NO_VALUE_TYPE,
    FUNC,
    VAR,
    LIT
} htab_value_type;

typedef enum {
    NO_DATA_TYPE,
    INT_T,
    FLOAT_T,
    STRING_T
} htab_data_type;

typedef union {
    int int_value;
    char *str_value;
    float float_value;
} htab_value;



/**
 * @brief Data container of the element
 * 
 * Fields are filled based on the value type:
 *  - FUNC:
 *      key             -- name of the function
 *      data_type       -- type of return value
 *      params_count    -- number function params
 * 
 *  - VAR:
 *      key             -- name of the variable
 *      data_type       -- type of the variable
 *      value           -- value of the variable
 * 
 *  - LIT:
 *      key             -- TODO
 *      data_type       -- type of the literal
 *      value           -- value of the literal
 * 
 */
typedef struct htab_data {
    htab_value_type type;
    htab_data_type data_type;
    int params_count;
    htab_value value;
} htab_data_t;


typedef struct htab_pair {
    htab_key_t    key;
    htab_data_t  value;
} htab_pair_t; 


struct htab;
typedef struct htab htab_t;


/**
 * @brief Hashes keys of the table
 * 
 * @param str 
 * @return size_t 
 */
size_t htab_hash_function(htab_key_t str);

/**
 * @brief Inicialization of a table
 * 
 * @param n Starting number of elememnts
 * @return htab_t* 
 */
htab_t * htab_init(size_t n);

/**
 * @brief Table destructor
 * 
 * @param t hash table 
 */
void htab_free(htab_t * t);

/**
 * @brief Returns pointer to the key-value pair in t, NULL if not found
 * 
 * @param t hash table 
 * @param key key of item 
 * @return htab_pair_t* 
 */
htab_pair_t * htab_find(htab_t * t, htab_key_t key);

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
 * @brief Prints table
 * 
 * @param t hash table 
 */
void htab_print(htab_t * t);

/**
 * @brief Returns pointer to the key-value pair in t, adds new key-value pair if not found
 * 
 * @param t hash table 
 * @param key key of item 
 * @return htab_pair_t* 
 */
htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key);

/**
 * @brief Increases / decreases size of t
 * 
 * @param t hash table 
 * @param n 
 * @return void* 
 */
void * htab_resize(htab_t *t, size_t n);

/**
 * @brief Destructs key-value pair. Returns false if not found
 * 
 * @param t hash table 
 * @param key key of item 
 * @return true 
 * @return false 
 */
bool htab_erase(htab_t * t, htab_key_t key);

/**
 * @brief Deletes all elements
 * 
 * @param t hash table 
 */
void htab_clear(htab_t * t);

/**
 * @brief Returns new instance of the table
 * 
 * @param t hash table 
 * @return htab_t* 
 */
htab_t * htab_copy(htab_t t);



#endif