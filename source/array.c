// Copyright Sean Kahler. All Rights Reserved.

#include "array.h"

#include <stdlib.h>

struct RTArrayT
{
    size_t typeSize;
    size_t size;
    void** elements;
};

RTErrorT RT_ArrayCreate(size_t typeSize, RTArrayT** outArr)
{
    RTArrayT* arr = malloc(sizeof(RTArrayT));
    if (arr == NULL)
    {
        return RT_ERROR_ALLOCATION_FAILED;
    }

    arr->typeSize = typeSize;
    arr->size = 0;
    arr->elements = calloc(0, typeSize);

    if (outArr != NULL)
    {
        *outArr = arr;
    }

    return RT_SUCCESS;
}

RTErrorT _ArrayRealloc(RTArrayT* arr)
{
    if (arr->size == 0)
    {
        RT_SafeFree(arr->elements);
        arr->elements = calloc(0, arr->typeSize);
        return RT_SUCCESS;
    }

    void* newBuf = realloc(arr->elements, arr->size * arr->typeSize);
    if (newBuf == NULL)
    {
        RT_ArrayDestroy(arr);
        return RT_ERROR_ALLOCATION_FAILED;
    }

    arr->elements = newBuf;
    return RT_SUCCESS;
}

RTErrorT RT_ArrayAppend(RTArrayT* arr, void* element)
{
    ++arr->size;

    RT_SAFE_RET(_ArrayRealloc(arr));
    arr->elements[arr->size - 1] = element;

    return RT_SUCCESS;
}

RTErrorT RT_ArrayRemove(RTArrayT* arr, size_t index)
{
    --arr->size;

    // Null out the index manually if size is now 0
    if (arr->size == 0)
    {
        arr->elements[index] = NULL;
    }

    for (size_t i = index; i < arr->size; ++i)
    {
        arr->elements[i] = arr->elements[(i + 1)];
    }

    RT_SAFE_RET(_ArrayRealloc(arr));
    return RT_SUCCESS;
}

size_t RT_ArrayGetSize(RTArrayT* arr)
{
    return arr->size;
}

void* RT_ArrayGetElement(RTArrayT* arr, size_t index)
{
    if (arr->size < index)
    {
        return NULL;
    }

    return arr->elements[index];
}

void RT_ArrayDestroy(RTArrayT* arr)
{
    if (arr == NULL)
    {
        return;
    }

    RT_SafeFree(arr->elements);
    free(arr);
}