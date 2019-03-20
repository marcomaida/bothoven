#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEW_ARRAY(type, size) malloc(size*sizeof(type)) 

#define NEW(type) malloc(sizeof(type))

#define FREE(obj) free(obj)

#define CLEAR_CONSOLE() printf("\e[1;1H\e[2J")