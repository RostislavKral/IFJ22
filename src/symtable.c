/**
 * @file sym_table_htab.c
 * @author Nikita Sniehovskyi
 * @brief Table of symbols based on hash table
 * @date 2022-10-22
 */

#include "symtable.h"

htab_t *htab_init(size_t n)
{
    if (n < 10)
    {
        n = 10;
    }

    htab_t *ht;

    if ((ht = malloc(sizeof(htab_t))) == NULL)
    {
        exit(99);
        return NULL;
    }

    ht->size = 0;
    ht->arr_size = n;

    if ((ht->arr_ptr = malloc(n * sizeof(htab_link_t *))) == NULL)
    {
        exit(99);
        return NULL;
    }

    // NULLs every pointer in elements
    for (size_t i = 0; i < ht->arr_size; i++)
    {
        ht->arr_ptr[i] = malloc(sizeof(htab_link_t));
        (ht->arr_ptr[i])->item = NULL;
    }

    return ht;
}

void htab_free(htab_t *t)
{
    htab_item_t *item = NULL;
    htab_item_t *item_erase = NULL;

    for (size_t i = 0; i < t->arr_size; i++)
    {
        item = t->arr_ptr[i]->item;
        while (item != NULL)
        {
            item_erase = item;
            item = item->next;
            free(item_erase->data.data_type);
            free(item_erase);
            item_erase = NULL;
        }
        t->arr_ptr[i]->item = NULL;
    }

    for (size_t i = 0; i < t->arr_size; i++)
    {
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

bool htab_insert_var(htab_t *t, char *name, int scope, enum T_KEYWORD type, htab_value value)
{
    htab_item_t *new_item = malloc(sizeof(htab_item_t));
    if (new_item == NULL) {
        exit(99);
    }

    int index = htab_hash_function(name) % t->arr_size;

    new_item->key = name;
    new_item->scope = scope;

    new_item->data.type = VAR;
    new_item->data.data_type = malloc(sizeof(enum T_KEYWORD));
    if (new_item->data.data_type == NULL) {
        exit(99);
    }

    (new_item->data.data_type)[0] = type;
    new_item->data.value = value;

    new_item->next = (t->arr_ptr[index])->item;
    (t->arr_ptr[index])->item = new_item;

    t->size += 1;

    if ((double)t->size / (double)t->arr_size > MAX_SPACE_TAKEN)
    {
        htab_resize(t, t->arr_size * 2);
    }

    return true;
}

// TODO can return lower scope if it wasn't cleared
htab_item_t *htab_find_var(htab_t *t, char *key, int scope)
{
    int index = htab_hash_function(key) % t->arr_size;
    htab_item_t *item = t->arr_ptr[index]->item;

    while (item != NULL && !(strcmp(key, item->key) == 0 && item->data.type == VAR))
    {
        item = item->next;
    }

    return item;
}

bool htab_insert_func(htab_t *t, char *name, enum T_KEYWORD ret_val_type, int params_count, enum T_KEYWORD *params_type)
{
    htab_item_t *new_item = malloc(sizeof(htab_item_t));
    if (new_item == NULL) {
        exit(99);
    }

    int index = htab_hash_function(name) % t->arr_size;

    new_item->key = name;
    new_item->scope = 0;
    new_item->data.type = FUNC;
    new_item->data.params_count = params_count;

    new_item->data.data_type = malloc((params_count + 1) * sizeof(enum T_KEYWORD));
    if (new_item->data.data_type == NULL) {
        exit(99);
    }
    
    (new_item->data.data_type)[0] = ret_val_type;

    for (int i = 1; i < params_count + 1; i++)
    {
        (new_item->data.data_type)[i] = params_type[i - 1];
    }

    if ((t->arr_ptr[index])->item == NULL)
    {
        (t->arr_ptr[index])->item = new_item;
    }
    else
    {
        htab_item_t *p = (t->arr_ptr[index])->item;
        while (p->next != NULL)
        {
            if (strcmp(p->key, name) == 0 && p->data.type == FUNC)
            {
                free(new_item->data.data_type);
                free(new_item);
                return false;
            }
            p = p->next;
        }

        p->next = new_item;
    }

    t->size += 1;

    if ((double)t->size / (double)t->arr_size > MAX_SPACE_TAKEN)
    {
        htab_resize(t, t->arr_size * 2);
    }

    return true;
}

htab_item_t *htab_find_func(htab_t *t, char *key)
{
    int index = htab_hash_function(key) % t->arr_size;
    htab_item_t *item = t->arr_ptr[index]->item;

    while (item != NULL && !(strcmp(key, item->key) == 0 && item->data.type == FUNC))
    {
        item = item->next;
    }

    return item;
}

bool htab_remove_scope(htab_t *t, int scope)
{
    for (size_t i = 0; i < t->arr_size; i++)
    {

        while ((t->arr_ptr[i])->item != NULL && (t->arr_ptr[i])->item->scope >= scope)
        {
            htab_item_t *item = (t->arr_ptr[i])->item->next;
            free((t->arr_ptr[i])->item->data.data_type);
            free((t->arr_ptr[i])->item);
            (t->arr_ptr[i])->item = item;
            t->size -= 1;
        }
    }

    if ((double)t->size / (double)t->arr_size < MIN_SPACE_TAKEN)
    {
        int new_n = (t->arr_size / 2 < 10) ? 10 : t->arr_size / 2;
        htab_resize(t, new_n);
    }

    return true;
}

size_t htab_items_count(const htab_t *t)
{
    return t->size;
}

size_t htab_size(const htab_t *t)
{
    return t->arr_size;
}

void htab_resize(htab_t *t, size_t n)
{
    if (n < 10)
    {
        n = 10;
    }

    htab_link_t **new_arr;
    if ((new_arr = malloc(n * sizeof(htab_link_t *))) == NULL)
    {
        exit(99);
    }

    // NULLs every pointer in elements
    for (size_t i = 0; i < n; i++)
    {
        new_arr[i] = malloc(sizeof(htab_link_t));
        (new_arr[i])->item = NULL;
    }

    htab_item_t *item;
    htab_item_t *place;
    for (size_t i = 0; i < t->arr_size; i++)
    {
        item = (t->arr_ptr)[i]->item;
        while (item != NULL)
        {
            int index = htab_hash_function(item->key) % n;

            place = (new_arr[index])->item;
            if (place == NULL)
            {
                (new_arr[index])->item = item;
                item = item->next;
                (new_arr[index])->item->next = NULL;
            }
            else
            {
                while (place->next != NULL)
                {
                    place = place->next;
                }

                place->next = item;
                item = item->next;
                place->next->next = NULL;
            }
        }
    }
    // Free array
    for (int i = 0; i < t->arr_size; i++)
    {
        free(t->arr_ptr[i]);
    }
    free(t->arr_ptr);
    t->arr_ptr = new_arr;
    t->arr_size = n;
}

void htab_debug_print(htab_t *t)
{
    printf("htab size: %d, currentry have %d item(s)\n", t->arr_size, t->size);

    for (int i = 0; i < t->arr_size; i++)
    {
        htab_item_t *ptr = t->arr_ptr[i]->item;
        while (ptr != NULL)
        {
            printf("[Key: %s, Scope %d]->", ptr->key, ptr->scope);
            ptr = ptr->next;
        }
        printf("NULL\n");
    }
}

size_t htab_hash_function(const char *str)
{
    size_t h = 0;

    for (size_t i = 0; i < strlen(str); i++)
    {
        h = h * 58293 + (int)str[i];
    }

    h = h * 58293;

    return h;
}