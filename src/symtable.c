/**
 * @file sym_table_htab.c
 * @author Nikita Sniehovskyi
 * @brief Table of symbols based on hash table
 * @date 2022-10-22
 */


#include "symtable.h"





htab_t * htab_init(size_t n) {
    if (n < 1) {
        exit(99);
        return NULL;
    }

    htab_t * ht;
    
    if ((ht = malloc(sizeof(htab_t))) == NULL) {
        exit(99);
        return NULL;
    }

    ht->size = 0;
    ht->arr_size = n;

    if ((ht->arr_ptr = malloc(n * sizeof(htab_link_t *))) == NULL) {
        exit(99);
        return NULL;
    }

    // NULLs every pointer in elements
    for (size_t i = 0; i < ht->arr_size; i++) {
        ht->arr_ptr[i] = malloc(sizeof(htab_link_t));
        (ht->arr_ptr[i])->item = NULL;
    }

    return ht;
}

// TODO same key same scope


// TODO free data_type
void htab_free(htab_t * t) {
    htab_item_t *item = NULL;
    htab_item_t *item_erase = NULL;

    for (size_t i = 0; i < t->arr_size; i++) {        
        item = t->arr_ptr[i]->item;
        while (item != NULL) {
            item_erase = item;
            item = item->next;
            // TODO clear data
            free(item_erase);
            item_erase = NULL;
        }
        t->arr_ptr[i]->item = NULL;
    }

    for (size_t i = 0; i < t->arr_size; i++) {        
        free(t->arr_ptr[i]);
        t->arr_ptr[i] = NULL;
    }

    free(t->arr_ptr);
    t->arr_ptr = NULL;

    t->arr_size = 0;
    t->size = 0;
    
    free(t);
    t = NULL;
}


bool htab_insert_var(htab_t * t, char * name, int scope, htab_data_type type, htab_value value) {
    htab_item_t * new_item = malloc(sizeof(htab_item_t));

    int index = htab_hash_function(name) % t->arr_size;

    new_item->key   = name;
    new_item->scope = scope;

    new_item->data.type      = VAR;
    new_item->data.data_type = malloc(sizeof(htab_data_type));
    *(new_item->data.data_type) = type;
    new_item->data.value     = value;

    new_item->next = (t->arr_ptr[index])->item; 
    (t->arr_ptr[index])->item = new_item;

    t->size += 1;

    return true;
}


// TODO can return lower scope if it wasn't cleared
htab_item_t * htab_find_var(htab_t * t, char * key, int scope) {
    int index = htab_hash_function(key) % t->arr_size;
    htab_item_t * item = t->arr_ptr[index]->item;

    while (item != NULL && !(strcmp(key, item->key) == 0 && item->data.type == VAR)) {
        item = item->next;
    }

    return item;
}



bool htab_insert_func(htab_t * t, char * name, htab_data_type ret_val_type, int params_count, htab_data_type * params_type) {
    htab_item_t * new_item = malloc(sizeof(htab_item_t));

    int index = htab_hash_function(name) % t->arr_size;

    new_item->key               = name;
    new_item->scope             = 0;
    new_item->data.type         = FUNC;
    new_item->data.params_count = params_count;
    
    new_item->data.data_type      = malloc((params_count + 1) * sizeof(htab_data_type));
    (new_item->data.data_type)[0] = ret_val_type;

    for (int i = 1; i < params_count + 1; i++) {
        (new_item->data.data_type)[i] = params_type[i-1];
    }

    if ((t->arr_ptr[index])->item == NULL) {
        (t->arr_ptr[index])->item = new_item;
    } else {
        htab_item_t * p = (t->arr_ptr[index])->item;
        while(p->next != NULL) {
            if (strcmp(p->key, name) == 0 && p->data.type == FUNC) {
                free(new_item->data.data_type);
                free(new_item);
                return false;
            }
            p = p->next;
        }

        p->next = new_item;
    }

    t->size += 1;

    return true;
}



htab_item_t * htab_find_func(htab_t * t, char * key) {
    int index = htab_hash_function(key) % t->arr_size;
    htab_item_t * item = t->arr_ptr[index]->item;

    while (item != NULL && !(strcmp(key, item->key) == 0 && item->data.type == FUNC)) {
        item = item->next;
    }

    return item;
}





bool htab_remove_scope(htab_t * t, int scope) {
    for (size_t i = 0; i < t->arr_size; i++) {
        
        while ((t->arr_ptr[i])->item != NULL && (t->arr_ptr[i])->item->scope >= scope) {
            htab_item_t * item = (t->arr_ptr[i])->item->next;
            free((t->arr_ptr[i])->item);
            (t->arr_ptr[i])->item = item;
            t->size -= 1;
        }
    }

    return true;
}




/*


htab_item_t * htab_find_item(htab_t * t, htab_key_t key) {
    htab_item_t *item;

    item = t->arr_ptr[htab_hash_function(key) % (t->arr_size)];

    if (item == NULL) {
        return NULL;
    }

    while (item->pair.key != key && item->next != NULL) {
        item = item->next;
    }

    if (item->pair.key == key) {
        return item;
    }
    
    return NULL;
}





htab_pair_t * htab_find(htab_t * t, htab_key_t key) {
    htab_item_t *item;

    item = htab_find_item(t, key);
    
    if (item != NULL) {
        return &item->pair;
    }

    return NULL;
}





size_t htab_items_count(const htab_t * t) {
    return t->size;
}





size_t htab_size(const htab_t * t) {
    return t->arr_size;
}





void htab_print(htab_t * t) {
    printf("array size: %ld\nelements count: %ld\n", t->arr_size, t->size);
    for (size_t i = 0; i < t->arr_size; i++) {
        htab_item_t *item = t->arr_ptr[i]; 
        while (item != NULL) {
            printf("[\"%s\": %d]->", item->pair.key, item->pair.value);
            item = item->next;
        }
        printf("NULL\n");
    }
}





htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key) {
    htab_item_t *item;
    
    item = htab_find_item(t, key);

    if (item != NULL) {
        return &item->pair;
    }    

    if ((item = (htab_item_t *) malloc(sizeof(htab_item_t))) == NULL) {
        // TODO Memory alloc error
        return NULL;
    }
    item->pair.key = key;


    item->pair.value.data_type = NO_DATA_TYPE;
    item->pair.value.params_count = -1;
    item->pair.value.type = NO_VALUE_TYPE;
    item->pair.value.value.int_value = -1;

    // Insert item at the start of chain
    item->next = t->arr_ptr[htab_hash_function(key) % (t->arr_size)];
    t->arr_ptr[htab_hash_function(key) % (t->arr_size)] = item;

    t->size++;

    // Check if table is not overfilled/underfilled
    if ((double)t->size / (double)t->arr_size > MAX_SPACE_TAKEN) {
        htab_resize(t, t->arr_size * 2);
    }

    return &item->pair;
}





void * htab_resize(htab_t *t, size_t n) {
     if (n < 1) {
        // TODO Bad size of array error
        return NULL;
    }

    htab_item_t **new_arr_ptr;
    if ((new_arr_ptr = (htab_item_t **) malloc(n * sizeof(htab_item_t *))) == NULL) {
        // TODO Memory alloc error
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        new_arr_ptr[i] = NULL;
    }

    htab_item_t *item;
    for (size_t i = 0; i < t->arr_size; i++) {
        item = t->arr_ptr[i];
        while(item != NULL) {
            htab_item_t *tmp;
            tmp = item->next;
            item->next = new_arr_ptr[htab_hash_function(item->pair.key) % n];
            new_arr_ptr[htab_hash_function(item->pair.key) % n] = item;
            item = tmp;
        }
    }

    // Free array
    free(t->arr_ptr);
    t->arr_ptr = new_arr_ptr;

    t->arr_size = n;

    return (void *)0;
}





bool htab_erase(htab_t * t, htab_key_t key) {
    htab_item_t *item;

    item = t->arr_ptr[htab_hash_function(key) % (t->arr_size)];
    if (item == NULL) {
        return false;
    }    
    
    // This will save pointer to the item, so when certain key will be found, it will be able to free it and connect previous item with the next one
    htab_item_t *item_last = NULL;

    if (item->pair.key != key) {
        item_last = item;
        item = item->next;

        while (item->pair.key != key && item->next != NULL) {
            item_last = item; // Contains pointer to the current element 
            item = item->next;
        }
    }

    // If wasn't found
    if (item == NULL) {
        return false;
    }

    // If it was first element
    if (item_last == NULL) {
        t->arr_ptr[htab_hash_function(key) % (t->arr_size)] = item->next;
        free(item);
    } else {
        item = item->next;
        free(item_last->next);
        item_last->next = item;
    }
    
    t->size--;

    if ((double)t->size / (double)t->arr_size < MIN_SPACE_TAKEN) {
        int new_n = (t->arr_size / 2 < 1) ? 1 : t->arr_size / 2;
        htab_resize(t, new_n);
    }

    return true;
}





void htab_clear(htab_t * t) {
    htab_item_t *item = NULL;
    htab_item_t *item_erase = NULL;

    // Free each element in the array
    for (size_t i = 0; i < t->arr_size; i++) {        
        item = t->arr_ptr[i];
        while (item != NULL) {
            item_erase = item;
            item = item->next;
            free(item_erase);
            item_erase = NULL;
        }
        t->arr_ptr[i] = NULL;
    }

    t->size = 0;
}





htab_t * htab_copy(htab_t t) {
    htab_t *new_htab = htab_init(t.arr_size);

    htab_item_t *item;
    htab_item_t *new_item;
    for (size_t i = 0; i < t.arr_size; i++) {
        item = t.arr_ptr[i];
        while(item != NULL) {
            if ((new_item = (htab_item_t *) malloc(sizeof(htab_item_t))) == NULL) {
                // TODO Memory alloc error
                return NULL;
            }
            new_item->pair.key = item->pair.key;
            new_item->pair.value = item->pair.value;

            new_item->next = new_htab->arr_ptr[htab_hash_function(new_item->pair.key) % (new_htab->arr_size)];
            new_htab->arr_ptr[htab_hash_function(new_item->pair.key) % (new_htab->arr_size)] = item;
        }
    }    

    return new_htab;
}







*/



void htab_debug_print(htab_t *t) {
    printf("htab size: %d, currentry have %d item(s)\n", t->arr_size, t->size);

    for (int i = 0; i < t->arr_size; i++) {
        htab_item_t *ptr = t->arr_ptr[i]->item;
        while(ptr != NULL) {
            printf("[Key: %s, Scope %d]->", ptr->key, ptr->scope);
            ptr = ptr->next;
        }
        printf("NULL\n");
    }
}

size_t htab_hash_function(const char *str) {
    size_t h=0;

    for (size_t i = 0; i < strlen(str); i++) {
        h = h*58293 + (int)str[i];
    }

    h = h*58293;

    return h;
}