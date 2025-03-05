#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define MAX_LEVELS 10  
#define MEMORY_SIZE (1 << MAX_LEVELS)  

typedef struct Block {
    size_t size;
    int free;
    struct Block *next;
} Block;

static uint8_t memory_pool[MEMORY_SIZE]; // Custom memory pool
Block *free_list[MAX_LEVELS + 1]; 
void *allocated_blocks[MEMORY_SIZE];  
int allocation_index = 0;

Block *create_block(size_t size) {
    static int initialized = 0;
    if (!initialized) {
        Block *initial_block = (Block *)memory_pool;
        initial_block->size = MEMORY_SIZE;
        initial_block->free = 1;
        initial_block->next = NULL;
        free_list[MAX_LEVELS] = initial_block;
        initialized = 1;
    }
    return (Block *)memory_pool;
}

void init_memory() {
    for (int i = 0; i <= MAX_LEVELS; i++) {
        free_list[i] = NULL;
    }
    for (int i = 0; i < MEMORY_SIZE; i++) {
        allocated_blocks[i] = NULL;
    }
    create_block(MEMORY_SIZE);
}

int get_level(size_t size) {
    int level = 0;
    while ((1 << level) < size) {
        level++;
    }
    return level;
}

void split(int level) {
    if (!free_list[level]) return;
    
    Block *block = free_list[level];
    free_list[level] = block->next;

    size_t new_size = block->size / 2;
    Block *buddy = (Block *)((uint8_t *)block + new_size);

    block->size = new_size;
    buddy->size = new_size;
    block->free = 1;
    buddy->free = 1;

    block->next = buddy;
    buddy->next = free_list[level - 1];
    free_list[level - 1] = block;
}

void *allocate(size_t size) {
    int level = get_level(size);
    while (level <= MAX_LEVELS && !free_list[level]) {
        level++;
    }
    if (level > MAX_LEVELS) return NULL;

    while (level > get_level(size)) {
        split(level);
        level--; 
    }

    Block *block = free_list[level];
    free_list[level] = block->next;
    block->free = 0;
    allocated_blocks[allocation_index++] = block;
    return (void *)block;
}

Block *find_buddy(Block *block) {
    size_t block_address = (size_t)block;
    size_t buddy_address = block_address ^ block->size;
    return (Block *)buddy_address;
}

void merge(int level) {
    Block *current = free_list[level];
    Block *prev = NULL;

    while (current) {
        Block *buddy = find_buddy(current);
        Block *search = free_list[level];
        Block *search_prev = NULL;
        while (search) {
            if (search == buddy && search->free) break;
            search_prev = search;
            search = search->next;
        }
        if (search) {
            if (prev) prev->next = current->next;
            else free_list[level] = current->next;

            if (search_prev) search_prev->next = search->next;
            else free_list[level] = search->next;

            if (current < buddy) {
                current->size *= 2;
                current->next = free_list[level + 1];
                free_list[level + 1] = current;
            } else {
                buddy->size *= 2;
                buddy->next = free_list[level + 1];
                free_list[level + 1] = buddy;
            }
            current = free_list[level];
            prev = NULL;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void free_memory(void *ptr) {
    if (!ptr) {
        printf("Invalid pointer.\n");
        return;
    }
    int found = 0;
    for (int i = 0; i < allocation_index; i++) {
        if (allocated_blocks[i] == ptr) {
            found = 1;
            allocated_blocks[i] = NULL;
            break;
        }
    }
    if (!found) {
        printf("Error: Pointer not found.\n");
        return;
    }
    Block *block = (Block *)ptr;
    block->free = 1;
    int level = get_level(block->size);
    block->next = free_list[level];
    free_list[level] = block;
    merge(level);
}

void display_memory() {
    printf("\n================ Free Memory Blocks ================\n");
    for (int i = 0; i <= MAX_LEVELS; i++) {
        printf("Level %d: ", i);
        Block *current = free_list[i];
        while (current) {
            printf("[Size: %zu] -> ", current->size);
            current = current->next;
        }
        printf("NULL\n");
    }
    printf("===================================================\n");
}

void display_allocated_memory() {
    printf("Allocated Memory Blocks:\n");
    for (int i = 0; i < allocation_index; i++) {
        if (allocated_blocks[i] != NULL) {
            Block *block = (Block *)allocated_blocks[i];
            printf("Index %d: Address = %p, Size = %zu\n", i, (void *)block, block->size);
        }
    }
}

int main() {
    int choice;
    size_t size;
    void *ptr;
    init_memory();
    while (1) {
        printf("\nBuddy System Memory Management\n");
        printf("1. Allocate Memory\n");
        printf("2. Free Memory\n");
        printf("3. Display Free Memory\n");
        printf("4. Display Allocated Memory\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("Enter size to allocate: ");
                scanf("%zu", &size);
                ptr = allocate(size);
                printf(ptr ? "Memory allocated at %p\n" : "Allocation failed.\n", ptr);
                break;
            case 2:
                printf("Enter memory index to free: ");
                int index;
                scanf("%d", &index);
                if (index < 0 || index >= allocation_index || !allocated_blocks[index])
                    printf("Invalid allocation index.\n");
                else {
                    free_memory(allocated_blocks[index]);
                    printf("Memory freed.\n");
                }
                break;
            case 3:
                display_memory();
                break;
            case 4:
                display_allocated_memory();
                break;
            case 5:
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}
