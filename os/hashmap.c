#include "FreeRTOS.h"
#include <hashmap.h>

int _getIndex(const char* key, int count) {
    /** Quick and dirty hash function **/
    int ret = 0, i;
    for(i = 0; key[i] != '\0' && i < KEY_LIMIT; i++) {
        ret += ((int)key[i]) & 0xFF;
    }
    return ret % count;
}

int _setItem(xHashMapItem_t* map, const char* key, void* item) {

    /** 
     ** Add or modify a hash entry 
     **  If the entry does not exist or is occupied by an existing
     **  value, return -1
     **/

    if(map->key[0] == '\0') {
        strncpy(map->key, key, KEY_LIMIT);
        map->data = item;
    } else if(strcmp(map->key, key) == 0) {
        map->data = item;
    } else {
        return -1;
    }

    return 1;
}


void xHashMapCreate(size_t member_size, int count, xHashMap_t* hmap) {

    /**
     ** Allocate space for a new hashmap.
     **/

    hmap->member_size = member_size;
    hmap->max_count = count;
    hmap->current_count = 0;
    hmap->map = (xHashMapItem_t*)pvPortMalloc(sizeof(xHashMapItem_t) * count);
    memset(hmap->map, 0, sizeof(xHashMapItem_t) * count);
}

int xHashMapSet(xHashMap_t* hmap, const char* key, void* value) {

    /**
     ** Set or modify a hash entry. Values are copied to 
     ** Specific space. 
     ** 
     ** NOTE: This function is NOT rentrant.
     **/

    // verify we have room
    if(hmap->current_count >= hmap->max_count) return -1;

    // allocate space for new member
    void* m = (void*)pvPortMalloc(hmap->member_size);

    // copy new member to allocated space
    memcpy(m, (void*)value, hmap->member_size);

    // hash the key
    int index = _getIndex(key, hmap->max_count);

    // Find the next space and set
    while(_setItem(&hmap->map[index], key, m) < 0) {
        if(index++ >= hmap->max_count) index = 0;
    }

    hmap->current_count++;
    return index;
}

void* xHashMapGet(xHashMap_t* hmap, const char* key) {

    /**
     ** Retrieve a refrence to a stored value by
     ** the given key.
     **/

    // hash the key
    int index = _getIndex(key, hmap->max_count);
    int _i = index;

    // Find the value starting at the hash offset
    do {

        if(strcmp(hmap->map[index].key, key) == 0)
            return hmap->map[index].data;

        if(_i++ >= hmap->max_count) _i = 0;

    } while(_i != index);

    return NULL;
}

int xHashMapKeys(xHashMap_t* hmap, char** buffer) {
    int i, r = 0;
    for(i = 0; i < hmap->max_count; i++) {
        if(hmap->map[i].key[0] == '\0') continue;
        *buffer++ = hmap->map[i].key;
        r++;
    }
    return r;
}

