#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEW_ARRAY(type, size) malloc(size*sizeof(type)); 

#define NEW(type) malloc(sizeof(type));