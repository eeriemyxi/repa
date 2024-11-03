#include <stdlib.h>

typedef struct {
    const char *key;
    int value;
} Node;

typedef struct {
    size_t capacity;
   	size_t length;
    Node *nodes;
} HashMap;

/* Initialize a hash map then return a pointer to it
 */
HashMap *HT_make_hash_map(void);

/* Return a hash capped to `cap` for a given `key`
 * Return the raw hash if `cap` is 0.
 */
unsigned int HT_hash(const char *key, const unsigned int cap);

/* Insert a key into the given hash map.
 */
void HT_insert(HashMap *map, char *key, signed int value);

/* Get value for a key from the given hash map.
 * Return -1 if key not found.
 */
signed int HT_get(HashMap *map, char *key);

/* Delete a key from given hash map.
 * Return -1 if key not found.
 * Return 0 if removed.
 */
signed char HT_delete(HashMap *map, char *key);

/* Free the given hash map from memory
 */
void HT_free_hash_map(HashMap *map);
