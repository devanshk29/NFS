#include "lru.h"

lru_entry *head = NULL;

ss_info find_in_cache(char *key)
{
    lru_entry *entry;
    HASH_FIND_STR(head, key, entry);
    if (entry)
    {
        // remove it (so the subsequent add will throw it on the front of the list)
        HASH_DELETE(hh, head, entry);
        HASH_ADD_KEYPTR(hh, head, entry->key, strlen(entry->key), entry);
        return entry->value;
    }
    ss_info ans;
    ans.ss_port = -5;
    return ans;
}

void add_to_cache(char *key, ss_info value)
{
    lru_entry *entry, *tmp_entry;
    entry = malloc(sizeof(lru_entry));
    entry->key = strdup(key);
    printf("key: %s\n", entry->key);
    entry->value = value;

    HASH_FIND_STR(head, key, tmp_entry);
    if (tmp_entry)
    {
        HASH_DELETE(hh, head, tmp_entry);
        free(tmp_entry->key);
        free(tmp_entry);
    }
    HASH_ADD_KEYPTR(hh, head, entry->key, strlen(entry->key), entry);

    // prune the cache to MAX_CACHE_SIZE
    if (HASH_COUNT(head) > MAX_CACHE_SIZE)
    {
        HASH_ITER(hh, head, entry, tmp_entry)
        {
            // prune the first entry (loop is based on insertion order so this deletes the oldest item)
            HASH_DELETE(hh, head, entry);
            free(entry->key);
            free(entry);
            break;
        }
    }
}

