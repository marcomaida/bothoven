#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENABLE_MEMORY_CHECK 1

#if ENABLE_MEMORY_CHECK
	unsigned long memoryAssignments;
	#define INIT_MEMORY_CHECK memoryAssignments = 0;
	#define CHECK_MEMORY printf("Memory leaks: %lu\n", memoryAssignments);

	#define NEW(type) (memoryAssignments++, malloc(sizeof(type)))
	#define NEW_ARRAY(type, size) (memoryAssignments += size,malloc(size*sizeof(type))) 
	#define FREE(obj) (memoryAssignments--, free(obj))
#else
	#define INIT_MEMORY_CHECK //do nothing
	#define CHECK_MEMORY //do nothing

	#define NEW(type) (malloc(sizeof(type)))
	#define NEW_ARRAY(type, size) (malloc(size*sizeof(type))) 
	#define FREE(obj) (free(obj))
#endif

#define FREE(obj) free(obj)

#define CLEAR_CONSOLE() printf("\e[1;1H\e[2J")