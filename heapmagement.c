/*
Heap Management System implemented using Buddy-system. 
(Division of blocks in powers of 2)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MEMORY_SIZE 1024
#define MIN_BLOCK_SIZE 16  

// Metadata for heap elements
typedef struct Block {
    size_t size;
    struct Block *next;
    bool allocated;
} Block;

// 1024 bytes memory to simulate Heap
char memory[MEMORY_SIZE];

// Free list for powers of 2 from 2^0 to 2^10
Block *freeList[11];  

// List to mantain allocated blocks
Block *allocatedList = NULL;

// Calculate ceil(log2(size)) to find the level in the free list
int getLevel(size_t size) {
    int level = 0;
    while ((1 << level) < size) {
        level++;
    }
    return level;
}

void initializeMemory() {
    for (int i = 0; i < 11; i++) freeList[i] = NULL;
    allocatedList = NULL;

    Block *initialBlock = (Block *)memory; // Typecasts char* to Block*
    initialBlock->size = MEMORY_SIZE;
    initialBlock->next = NULL;
    initialBlock->allocated = false;
    freeList[getLevel(MEMORY_SIZE)] = initialBlock;
}

// Splitting blocks in equal halves iteratively till free-block of required size not created
void splitBlock(int fromLevel, int toLevel) {
    while (fromLevel > toLevel) {
        Block *block = freeList[fromLevel];
        if (block){
            freeList[fromLevel] = block->next; // Remove the block

            // Creating new Block of half the size
            size_t newSize = block->size / 2;
            Block *buddy = (Block *)((char *)block + newSize); 

            block->size = newSize;
            buddy->size = newSize;
            block->allocated = false;
            buddy->allocated = false;

            // Insert both new blocks into the smaller free list
            block->next = buddy;
            buddy->next = freeList[fromLevel - 1];
            freeList[fromLevel - 1] = block;

            fromLevel--;
        }
    }
}

// Allocate a heap block of required size (best-fit)
void *allocateMemory(size_t size) {
    void *ret_val = NULL;
    size_t totalSize = size + sizeof(Block); // Adding metadata in required size
    if (totalSize < MIN_BLOCK_SIZE) totalSize = MIN_BLOCK_SIZE;

    int level = getLevel(totalSize);
    int i = level;

    // Finding first non-empty free-list for a level >= required level
    while (i < 11 && !freeList[i]) i++;
    if (i != 11) {
        // Split the Block until required block size not created
        splitBlock(i, level);

        Block *block = freeList[level];
        freeList[level] = block->next;
        block -> allocated = true;
        block -> next = allocatedList;
        allocatedList = block;

        printf("Allocated block of size %zu at address %p\n", block -> size, (void *)block);
        ret_val = (void *)((char *)block + sizeof(Block));  //  Directly return user memory
    }
    else {
        printf("Memory Allocation failed. Enough Space not available for required allocation\n");
    }
    return ret_val;
}


// Get buddy of a block
Block *getBuddy(Block *block) {
    size_t offset = (char *)block - memory;
    size_t buddyOffset = offset ^ block->size;
    // XOR being used to calcualte off-set due to binary-tree representation of buddy-system
    return (Block *)(memory + buddyOffset);
}

// Merge free blocks
void mergeBlock(Block *block) {
    int level = getLevel(block->size);
    Block *buddy = getBuddy(block);

    // Loop for multiple merging in case of consecutive levels blocks
    while (level < 10 && !buddy->allocated && buddy->size == block->size) {
        Block **prev = &freeList[level];

        // Finding location of buddy in level
        while (*prev && *prev != buddy){
            prev = &(*prev)->next;
        }
        if (*prev) {
            *prev = buddy->next; // Remove buddy from free list
        }

        if ((char *)block > (char *)buddy) {
            block = buddy;
        } 

        block->size *= 2;   // Doubling size after merging
        level++;    
        buddy = getBuddy(block);
    }
    block -> allocated = false;
    block -> next = freeList[level];    // Adding final-merged block to free-list (level)
    freeList[level] = block;
}

// Remove allocated block from allocated-list
void removeAllocatedBlock(Block *block) {
    Block **prev = &allocatedList;
    while (*prev && *prev != block) {
        prev = &(*prev)->next;
    }
    if (*prev) {
        *prev = block -> next;
    }
}

// Function to free memory block
void freeMemory(void *ptr) {
    if (!ptr) return;

    Block *block = (Block *)((char *)ptr - sizeof(Block));  // Recover block
    block -> allocated = false;

    removeAllocatedBlock(block);
    mergeBlock(block);
}


// Print free list state
void printFreeMemoryState() {
    printf("\nFree List:\n");
    for (int i = 0; i < 11; i++) {
        printf("Size %d: ", 1 << i);
        Block *current = freeList[i];
        while (current) {
            printf("[%p] ", (void *)current);
            current = current->next;
        }
        printf("\n");
    }
}

// Print allocated list
void printAllocatedMemory() {
    printf("\nAllocated Memory List:\n");
    Block *current = allocatedList;
    while (current) {
        printf("Address: %p, Size: %zu\n", (void *)((char *)current + sizeof(Block)), current->size);
        current = current -> next;
    }
}

int main() {
    initializeMemory(); 
    int choice = 0;
    size_t size;
    void *ptr;

    while (choice != 5) {
        printf("\n----- Virtual Heap Manager -----\n");
        printf("1. Allocate Memory\n");
        printf("2. View Free Memory List\n");
        printf("3. View Allocated Memory List\n");
        printf("4. Free Memory\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter size to allocate: ");
                scanf("%zu", &size);
                ptr = allocateMemory(size);
                if (ptr) {
                    printf("Memory allocated at address: %p\n", ptr);
                }
                break;

            case 2:
                printFreeMemoryState();
                break;

            case 3:
                printAllocatedMemory();
                break;

            case 4:
                // Displays user only the memory blocks that are allocated so he can free it
                if (!allocatedList) {
                    printf("No allocated memory to free!\n");
                }
                else{
                    printf("\nAllocated Blocks:\n");
                    Block *current = allocatedList;
                    int index = 1;
                    while (current) {
                        printf("%d. Address: %p, Size: %zu\n", index, 
                            (void *)((char *)current + sizeof(Block)), current->size);
                        current = current->next;
                        index++;
                    }

                    int blockToFree;
                    printf("Select the block to free (1-%d): ", index - 1);
                    scanf("%d", &blockToFree);

                    if (blockToFree < 1 || blockToFree >= index) {
                        printf("Invalid selection.\n");
                    }
                    else{
                        // Find the selected block
                        current = allocatedList;
                        for (int i = 1; i < blockToFree; i++) {
                            current = current->next;
                        }

                        freeMemory((void *)((char *)current + sizeof(Block)));
                        printf("Memory at address %p freed.\n", (void *)((char *)current + sizeof(Block)));
                    }
                }
                break;

            case 5:
                printf("Exiting program...\n");

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}
