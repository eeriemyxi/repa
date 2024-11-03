#include "hashmap.h"
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static const int INITIAL_SIZE = 500;

/* Initialize a hash map then return a pointer to it
 */
HashMap *HT_make_hash_map(void) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));

    map->capacity = INITIAL_SIZE;
    map->length = 0;
    map->nodes = (Node *)calloc(INITIAL_SIZE, sizeof(Node));

    return map;
}

/* Return a hash capped to `cap` for a given `key`
 * Return the raw hash if `cap` is 0.
 */
unsigned int HT_hash(const char *key, const unsigned int cap) {
    unsigned int code = 1;

    while (*key != '\0') {
        code *= *key * 31;
        key++;
    }

    if (cap == 0) {
        return code;
    }

    return code % cap;
}

void _HT_helper_insert(Node *nodes, size_t capacity, size_t *length, const char *key,
                       signed int *value, bool duplicate) {
    size_t index = HT_hash(key, capacity - 1);

    while (nodes[index].key != NULL) {
        if (strcmp(nodes[index].key, key) == 0) {
            nodes[index].value = *value;
            return;
        }
        index++;
        if (index >= capacity) {
            index = 0;
        }
    }

    if (duplicate == true) {
        key = strdup(key);
        if (key == NULL) {
            fprintf(stderr, "Couldn't duplicate key.");
            exit(1);
        }
        ++*length;
    }

    nodes[index].key = (char *)key;
    nodes[index].value = *value;

    return;
}

void _HT_expand(HashMap *map) {
    map->capacity = map->capacity * 2;
    Node *const nmem = calloc(map->capacity, sizeof(Node));

    if (nmem == NULL) {
        fprintf(stderr, "Expanding hashmap failed!");
        exit(1);
    }

    for (size_t i = 0; i < (map->capacity / 2); i++) {
        if (map->nodes[i].key != NULL) {
            _HT_helper_insert(nmem, map->capacity, &map->length, map->nodes[i].key,
                              &map->nodes[i].value, false);
        }
    }

    free(map->nodes);
    map->nodes = nmem;
}

/* Insert a key into the given hash map.
 */
void HT_insert(HashMap *map, char *key, signed int value) {
    if (map->length >= map->capacity / 2) {
        _HT_expand(map);
    }

    _HT_helper_insert(map->nodes, map->capacity, &map->length, key, &value, true);
}

/* Get value for a key from the given hash map.
 * Return -1 if key not found.
 */
signed int HT_get(HashMap *map, char *key) {
    size_t index = HT_hash(key, map->capacity - 1);
    while (map->nodes[index].key != NULL) {
        if (strcmp(map->nodes[index].key, key) == 0) {
            return map->nodes[index].value;
        }
        index++;
        if (index >= map->capacity) {
            index = 0;
        }
    }

    return -1;
}

/* Delete a key from given hash map.
 * Return -1 if key not found.
 * Return 0 if removed.
 */
// signed char HT_delete(HashMap *map, char *key) {
//     const int index = HT_hash(key, map->);
//     Node *node = (map->nodes)[index];

//     while (node != NULL) {
//         if (strcmp(node->key, key) == 0) {
//             free(node);
//             return 0;
//         }
//         node = (Node *)node->next_node;
//     };
//     return -1;
// }

/* Free the given hash map from memory
 */
void HT_free_hash_map(HashMap *map) {
    for (size_t i = 0; i < map->capacity; i++) {
        free((void *)map->nodes[i].key);
    }

    free(map->nodes);
    free(map);
}
