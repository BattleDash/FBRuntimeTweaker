// Copyright Sean Kahler. All Rights Reserved.

#pragma once

#include "util.h"
#include "array.h"

#include <stdint.h>

typedef enum
{
    FrostyResourceType_Invalid = 0,
    FrostyResourceType_Embedded,
    FrostyResourceType_Ebx,
    FrostyResourceType_Res,
    FrostyResourceType_Chunk,
    FrostyResourceType_Bundle
} RTFrostyResourceTypeE;

typedef struct
{
    RTFrostyResourceTypeE type;
    int32_t resourceIndex;

    char* name;
    char sha1[20];

    int64_t size;
    uint8_t flags;
    int32_t handleHash;

    char* userData;
} RTFrostyResourceT;

EXTERN_C_BLOCK_START

// Parse a single mod resource
RTErrorT RT_FrostyParseResource(const char** buf, RTFrostyResourceT* resource);

// Parse a list of mod resources
RTErrorT RT_FrostyParseResources(const char** buf, RTArrayT** outResources);

// Destroy a mod resource
void RT_FrostyDestroyResource(RTFrostyResourceT* resource);

// Decompress CAS-compressed EBX data
RTErrorT RT_FrostyDecompressEbx(const char* buf, size_t size, char** outBuf, size_t* outSize);

EXTERN_C_BLOCK_END