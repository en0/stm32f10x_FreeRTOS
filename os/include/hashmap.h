#ifndef __HASHMAP_H
#define __HASHMAP_H

#include <string.h>

#define KEY_LIMIT 16

typedef struct {
    char key[KEY_LIMIT];
    void* data;
} xHashMapItem_t;


typedef struct {
    xHashMapItem_t* map;
    size_t member_size;
    int max_count;
    int current_count;
} xHashMap_t;


void xHashMapCreate(size_t member_size, int count, xHashMap_t* hmap);
int xHashMapSet(xHashMap_t* hmap, const char* key, void* value);
void* xHashMapGet(xHashMap_t* hmap, const char* key);

#endif /** __HASHMAP_H **/

