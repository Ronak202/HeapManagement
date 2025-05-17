# Buddy System Memory Management

## Overview
This program implements a **Buddy System Memory Management** technique to efficiently allocate and free memory blocks. It simulates a memory pool and manages allocations using a linked list-based approach.

## Features
- **Memory Allocation**: Allocates memory blocks using the buddy system.
- **Memory Deallocation**: Frees allocated memory and merges adjacent free blocks.
- **Display Free Memory**: Shows the list of available memory blocks at each level.
- **Display Allocated Memory**: Lists the allocated memory blocks with their sizes and addresses.

## Implementation Details
- **Memory Pool**: A fixed-size memory pool of `2^MAX_LEVELS` bytes.
- **Buddy System**: Splits larger memory blocks into smaller buddies when needed.
- **Linked List**: Maintains free blocks at different levels.
- **Efficient Merging**: When memory is freed, adjacent buddies are merged.

## Functions
- `void *allocateMemory(size_t size)`: Allocates memory of given size.
- `void freeMemory(void *ptr)`: Frees an allocated block and merges if possible.
- `void printFreeMemoryState(): Displays the current state of free memory blocks.
- `void printAllocatedMemory()`: Shows allocated memory blocks and their details.

## Usage
1. Run the program.
2. Choose options from the menu:
   - `1` to allocate memory.
   - `2` to free memory.
   - `3` to display free memory blocks.
   - `4` to display allocated memory.
   - `5` to exit.

## Example Output
```
Buddy System Memory Management
1. Allocate Memory
2. Free Memory
3. Display Free Memory
4. Display Allocated Memory
5. Exit
Enter your choice: 1
Enter size to allocate: 128
Memory allocated at 0x600000
```

## Compilation
Compile using `gcc`:
```
gcc buddy_system.c -o buddy_system
./buddy_system
```

