/*
Name:kurra venkata siva
Date:
Description:
Sample I/P:
Sample O/P:
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define HEAP_SIZE 128 * 1024   // 128KB heap
#define MIN_BLOCK_SIZE 1024    // 1KB minimum block size

typedef struct Block 
{
    size_t size;        
    bool is_free;      
    struct Block *next;    
} Block;

uint8_t heap[HEAP_SIZE];
Block *free_list = NULL;

void init_heap() 
{
    free_list = (Block *) heap;
    free_list->size = HEAP_SIZE;
    free_list->is_free = true;
    free_list->next = NULL;
}

Block *find_best_fit(size_t size) 
{
    Block *best_fit = NULL;
    Block *current = free_list;

    while (current != NULL) 
    {
        if (current->is_free && current->size >= size) 
	{
            if (best_fit == NULL || current->size < best_fit->size) 
	    {
                best_fit = current;
            }
        }
        current = current->next;
    }

    return best_fit;
}

void split_block(Block *block, size_t size) 
{
    if (block->size >= size + sizeof(Block) + MIN_BLOCK_SIZE) 
    {
        Block *new_block = (Block *)((uint8_t *)block + size + sizeof(Block));
        new_block->size = block->size - size - sizeof(Block);
        new_block->is_free = true;
        new_block->next = block->next;

        block->size = size;
        block->next = new_block;
    }
}

void *malloc_custom(size_t size) 
{
    if (size <= 0) 
	return NULL;
    if (size < MIN_BLOCK_SIZE) 
    {
        size = MIN_BLOCK_SIZE;
    }
    Block *block = find_best_fit(size);
    if (block == NULL) 
    {
        printf("Error: No suitable block found\n");
        return NULL;
    }

 
    split_block(block, size);

    block->is_free = false;

    return (void *)((uint8_t *)block + sizeof(Block));
}

void merge_free_blocks() 
{
    Block *current = free_list;

    while (current != NULL && current->next != NULL) 
    {
        if (current->is_free && current->next->is_free) 
	{
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
        } 
	else 
	{
            current = current->next;
        }
    }
}

void free_custom(void *ptr) 
{
    if (ptr == NULL) 
	return;

    Block *block = (Block *)((uint8_t *)ptr - sizeof(Block));
    block->is_free = true;
    merge_free_blocks();
}

void print_heap_status() 
{
    Block *current = free_list;
    while (current != NULL) 
    {
        printf("Block at %p, Size: %zu, Free: %s\n", current, current->size, current->is_free ? "Yes" : "No");
        current = current->next;
    }
}

int main() 
{
    init_heap();

    void *ptr1 = malloc_custom(128 * sizeof(int));  // Should allocate 1KB
    print_heap_status();
    
    void *ptr2 = malloc_custom(1000 * sizeof(uint8_t));  // Should allocate 1KB
    print_heap_status();
    
    void *ptr3 = malloc_custom(128 * 8 * 1024);  // Should allocate 128KB
    print_heap_status();
    
    free_custom(ptr1);  
    print_heap_status();
    
    free_custom(ptr2);  
    print_heap_status();
    return 0;
}

