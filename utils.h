#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOOL char
#define TRUE 1
#define FALSE 0

#define ENABLE_MEMORY_CHECK 1
#define ENABLE_MEMORY_LOG 1

#if ENABLE_MEMORY_CHECK
	unsigned long memoryAssignments;
	unsigned long allocatedBytes;
	#define START_MEMORY_CHECK memoryAssignments = allocatedBytes = 0;
	#define STOP_MEMORY_CHECK printf("Memory leaks: %lu\n", memoryAssignments);

	#define MEMORY_CHECK_NEW(type) memoryAssignments++, allocatedBytes += sizeof(type),
	#define MEMORY_CHECK_FREE(type) memoryAssignments--, allocatedBytes += sizeof(type),
	#define MEMORY_CHECK_NEW_ARRAY(type, length) memoryAssignments += length, allocatedBytes += length * sizeof(type),
	#define MEMORY_CHECK_FREE_ARRAY(type, length) memoryAssignments -= length; allocatedBytes += length * sizeof(type);
#else
	#define START_MEMORY_CHECK 
	#define STOP_MEMORY_CHECK

	#define MEMORY_CHECK_NEW(type)
	#define MEMORY_CHECK_FREE(type)
	#define MEMORY_CHECK_NEW_ARRAY(type, length)
	#define MEMORY_CHECK_FREE_ARRAY(type, length)
#endif

#if ENABLE_MEMORY_LOG
	unsigned BOOL is_active;
	#define START_MEMORY_LOG is_active = TRUE;
	#define STOP_MEMORY_LOG is_active = FALSE;

	#define MEMORY_LOG_NEW(type) 						printf(is_active == TRUE ? "MEMLOG +++ %20s, %d bytes\n" : "", #type, sizeof(type)),
	#define MEMORY_LOG_FREE(type, obj) 					printf(is_active == TRUE ? "MEMLOG --- %20s, %d bytes\n" : "", #type, sizeof(type)),
	#define MEMORY_LOG_NEW_ARRAY(type, length) 			printf(is_active == TRUE ? "MEMLOG +++ %20s length: %d, %d bytes\n" : "", "[" #type "]", length, length * sizeof(type)),
	#define MEMORY_LOG_FREE_ARRAY(type, obj, length) 	printf(is_active == TRUE ? "MEMLOG --- %20s length: %d, %d bytes\n" : "", "[" #type "]", length, length * sizeof(type));
#else
	#define START_MEMORY_LOG 
	#define STOP_MEMORY_LOG

	#define MEMORY_LOG_NEW(type)
	#define MEMORY_LOG_FREE(type, obj)
	#define MEMORY_LOG_NEW_ARRAY(type, length) 
	#define MEMORY_LOG_FREE_ARRAY(type, obj, length) 
#endif

	#define NEW(type) 								( MEMORY_CHECK_NEW(type) MEMORY_LOG_NEW(type) malloc(sizeof(type)))
	#define FREE(type, obj) 						( MEMORY_CHECK_FREE(type) MEMORY_LOG_FREE(type, obj) free(obj))
	#define NEW_ARRAY(type, length) 				( MEMORY_CHECK_NEW_ARRAY(type, length) MEMORY_LOG_NEW_ARRAY(type,length) malloc(length*sizeof(type))) 
	#define FREE_ARRAY(type, obj, length) 			{ MEMORY_CHECK_FREE_ARRAY(type, length) MEMORY_LOG_FREE_ARRAY(type,obj,length) for(int rmidx = 0; rmidx < length; rmidx++) free(obj), obj++; }




#define CLEAR_CONSOLE() printf("\e[1;1H\e[2J")