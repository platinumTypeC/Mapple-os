#pragma once
#include <mapple/types.h>

void init_malloc();
void malloc_addblock(void* addr, size_t size);
void* malloc(size_t size);
void free(void* ptr);
