// Copyright Sean Kahler. All Rights Reserved.

#pragma once

#include "util.h"

typedef struct RTArrayT RTArrayT;

EXTERN_C_BLOCK_START

// Allocate a new array, with typeSize being the size of the type in the array.
// For example, an array of char would use sizeof(char).
RTErrorT RT_ArrayCreate(size_t typeSize, RTArrayT** outArr);

// Append an element to the array
RTErrorT RT_ArrayAppend(RTArrayT* arr, void* element);

// Append an element to the array
RTErrorT RT_ArrayRemove(RTArrayT* arr, size_t index);

// Retrieve an element in the array
size_t RT_ArrayGetSize(RTArrayT* arr);

// Retrieve an element in the array
void* RT_ArrayGetElement(RTArrayT* arr, size_t index);

// Destroy the array
void RT_ArrayDestroy(RTArrayT* arr);

EXTERN_C_BLOCK_END