#include <mapple/Memory.h>

#define align_up(num, align) (((num) + ((align)-1)) & ~((align)-1))
#define ALLOC_HEADER_SZ offsetof(alloc_node_t, block)

// We are enforcing a minimum allocation size of 32B.
#define MIN_ALLOC_SZ ALLOC_HEADER_SZ + 32

typedef struct
{
	ll_t node;
	size_t size;
	char* block;
} alloc_node_t;

__attribute__((weak)) void malloc_lock()
{
	// Intentional no-op
}

__attribute__((weak)) void malloc_unlock()
{
	// Intentional no-op
}

static LIST_INIT(free_list);

#define MEM_BLOCK_SIZE (1024 * 1024)
static uint8_t mem_block[MEM_BLOCK_SIZE];

void malloc_addblock(void* addr, size_t size)
{
	// let's align the start address of our block to the next pointer aligned number
	alloc_node_t* new_memory_block = (alloc_node_t*)align_up((uintptr_t)addr, sizeof(void*));

	// calculate actual size - remove our alignment and our header space from the availability
	new_memory_block->size = (uintptr_t)addr + size - (uintptr_t)new_memory_block - ALLOC_HEADER_SZ;

	// and now our giant block of memory is added to the list!
	malloc_lock();
	list_add(&new_memory_block->node, &free_list);
	malloc_unlock();
}

void init_malloc()
{
    malloc_addblock(mem_block, MEM_BLOCK_SIZE);
}

void* malloc(size_t size){
    void* ptr = NULL;
    alloc_node_t* blk = NULL;

	if(size > 0)
	{
		// Align the pointer
		size = align_up(size, sizeof(void*));

        malloc_lock();

        // iterate over every entry
        list_for_each_entry(blk, &free_list, node)
        {
            if (blk->size >= size)
            {
                ptr = &blk->block;
                break;
            }
        }

        // can we split it?
        if(ptr)
            {
			// Can we split the block?
			if((blk->size - size) >= MIN_ALLOC_SZ)
			{
				alloc_node_t* new_blk = (alloc_node_t*)((uintptr_t)(&blk->block) + size);
				new_blk->size = blk->size - size - ALLOC_HEADER_SZ;
				blk->size = size;
				list_insert(&new_blk->node, &blk->node, blk->node.next);
			}

			list_del(&blk->node);
		}

		malloc_unlock();
    }
    return ptr;
}

void defrag_free_list(void)
{
	alloc_node_t* block = NULL;
	alloc_node_t* last_block = NULL;
	alloc_node_t* temp = NULL;

	list_for_each_entry_safe(block, temp, &free_list, node)
	{
		if(last_block)
		{
			if((((uintptr_t)&last_block->block) + last_block->size) == (uintptr_t)block)
			{
				last_block->size += ALLOC_HEADER_SZ + block->size;
				list_del(&block->node);
				continue;
			}
		}
		last_block = block;
	}
}

void free(void* ptr)
{
	// Don't free a NULL pointer..
	if(ptr)
	{
		// we take the pointer and use container_of to get the corresponding alloc block
		alloc_node_t* current_block = container_of((char* const*)ptr, alloc_node_t, block);
		alloc_node_t* free_block = NULL;

		malloc_lock();

		// Let's put it back in the proper spot
		list_for_each_entry(free_block, &free_list, node)
		{
			if(free_block > current_block)
			{
				list_insert(&current_block->node, free_block->node.prev, &free_block->node);
				goto blockadded;
			}
		}
		list_add_tail(&current_block->node, &free_list);

	blockadded:
		// Let's see if we can combine any memory
		defrag_free_list();

		malloc_unlock();
	}
}
