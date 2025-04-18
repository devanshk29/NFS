#include "lru_caching.h"

// Define a structure for a cache node
struct CacheNode {
    char path[256];  // Assuming a maximum length of 255 characters for file paths
    // Add other relevant data as needed
    struct CacheNode* next;
    struct CacheNode* prev;
};

// Define a structure for the cache
struct LRUCache {
    int capacity;
    int size;
    struct CacheNode* head;
    struct CacheNode* tail;
    // Add other relevant data as needed
};

// Function to initialize the LRU cache
struct LRUCache* initLRUCache(int capacity) {
    struct LRUCache* cache = (struct LRUCache*)malloc(sizeof(struct LRUCache));
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = NULL;
    cache->tail = NULL;
    return cache;
}

// Function to remove the least recently used item from the cache
void removeLRU(struct LRUCache* cache) {
    if (cache->size > 0) {
        struct CacheNode* temp = cache->head;
        cache->head = cache->head->next;
        if (cache->head != NULL) {
            cache->head->prev = NULL;
        } else {
            cache->tail = NULL; // The cache is now empty
        }
        free(temp);
        cache->size--;
    }
}

// Function to move a node to the front of the cache (indicating it was recently used)
void moveToFront(struct LRUCache* cache, struct CacheNode* node) {
    if (node != cache->head) {
        // Remove node from its current position
        if (node->prev != NULL) {
            node->prev->next = node->next;
        } else {
            cache->head = node->next;
        }

        if (node->next != NULL) {
            node->next->prev = node->prev;
        } else {
            cache->tail = node->prev;
        }

        // Move node to the front
        node->next = cache->head;
        node->prev = NULL;

        if (cache->head != NULL) {
            cache->head->prev = node;
        }

        cache->head = node;

        // Update the tail if this is the first move (head was NULL before)
        if (cache->tail == NULL) {
            cache->tail = cache->head;
        }
    }
}

// Function to access a path in the cache
void accessPath(struct LRUCache* cache, const char* path) {
    // Check if the path is already in the cache
    struct CacheNode* current = cache->head;
    while (current != NULL) {
        if (strcmp(current->path, path) == 0) {
            // Move the accessed node to the front
            moveToFront(cache, current);
            return;
        }
        current = current->next;
    }

    // If the path is not in the cache, add it
    struct CacheNode* newNode = (struct CacheNode*)malloc(sizeof(struct CacheNode));
    strcpy(newNode->path, path);

    // Move the new node to the front
    newNode->next = cache->head;
    newNode->prev = NULL;

    if (cache->head != NULL) {
        cache->head->prev = newNode;
    }

    cache->head = newNode;

    // Update the tail if this is the first node in the cache
    if (cache->tail == NULL) {
        cache->tail = cache->head;
    }

    // If the cache is at capacity, remove the least recently used item
    if (cache->size == cache->capacity) {
        removeLRU(cache);
    } else {
        cache->size++;
    }
}

// Function to print the contents of the cache
void printCache(struct LRUCache* cache) {
    struct CacheNode* current = cache->head;
    while (current != NULL) {
        printf("%s ", current->path);
        current = current->next;
    }
    printf("\n");
}


int main() {
    // Jitna capacity chahiye utni cache bana sakte hai abhi 5 ka example hai
    struct LRUCache* cache = initLRUCache(10);
    //jaise hi koi path access hoga client ke through toh usko phele check karna hai cache me hai ya nhi agar nhi toh add karna hai
    accessPath(cache, "/path1");
    printCache(cache); // Output: /path1

    

    return 0;
}

