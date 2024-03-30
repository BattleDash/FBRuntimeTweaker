// Copyright Sean Kahler. All Rights Reserved.

#include "frosty_resource.h"

#include "parse_common.h"

#include <zstd.h>

#include <stdlib.h>
#include <string.h>

RTErrorT RT_FrostyParseResource(const char** buf, RTFrostyResourceT* resource)
{
    resource->type = 1 + RT_READ_BUF(uint8_t, buf);

    resource->resourceIndex = RT_READ_BUF(int32_t, buf);
    RT_SAFE_RET(RT_ReadTerminatedString(buf, &resource->name));

    if (resource->resourceIndex != -1)
    {
        RT_ReadSha1(buf, resource->sha1);

        resource->size = RT_READ_BUF(int64_t, buf);
        resource->flags = RT_READ_BUF(uint8_t, buf);
        resource->handleHash = RT_READ_BUF(int32_t, buf);

        RT_SAFE_RET(RT_ReadTerminatedString(buf, &resource->userData));
    }

    RT_SAFE_RET(RT_ArrayCreate(sizeof(uintptr_t), &resource->addedBundles));

    int32_t count = RT_READ_BUF(int32_t, buf);
    for (int32_t i = 0; i < count; ++i)
    {
        uintptr_t bundleHash = RT_READ_BUF(uint32_t, buf);
        RT_SAFE_RET(RT_ArrayAppend(resource->addedBundles, (void*)bundleHash));
    }

    // Special handling for bundle/res/chunk resources
    if (resource->type == FrostyResourceType_Bundle)
    {
        RT_SAFE_RET(RT_ReadTerminatedString(buf, &resource->name));
        RT_UNUSED(RT_READ_BUF(int32_t, buf));
    }
    else if (resource->type == FrostyResourceType_Res)
    {
        resource->resType = RT_READ_BUF(uint32_t, buf);
        resource->resRid = RT_READ_BUF(uint64_t, buf);
        int32_t metaSize = RT_READ_BUF(int32_t, buf);
        memcpy(resource->resMeta, *buf, metaSize);
        *buf += metaSize;
    }
    else if (resource->type == FrostyResourceType_Chunk)
    {
        resource->rangeStart = RT_READ_BUF(uint32_t, buf);
        resource->rangeEnd = RT_READ_BUF(uint32_t, buf);
        resource->logicalOffset = RT_READ_BUF(uint32_t, buf);
        resource->logicalSize = RT_READ_BUF(uint32_t, buf);
        resource->h32 = RT_READ_BUF(int32_t, buf);
        resource->firstMip = RT_READ_BUF(int32_t, buf);

        if (resource->firstMip == -1 && resource->rangeStart != 0)
        {
            resource->firstMip = 0;
        }
    }

    return RT_SUCCESS;
}

RTErrorT RT_FrostyParseResources(const char** buf, RTArrayT** outResources)
{
    int32_t count = RT_READ_BUF(int32_t, buf);

    RTArrayT* arr;
    RT_SAFE_RET(RT_ArrayCreate(sizeof(RTFrostyResourceT*), &arr));

    RT_LogF("Reading %d resources", count);

    for (int32_t i = 0; i < count; ++i)
    {
        RTFrostyResourceT* resource = malloc(sizeof(RTFrostyResourceT));
        if (resource == NULL)
        {
            return RT_ERROR_ALLOCATION_FAILED;
        }

        memset(resource, 0, sizeof(RTFrostyResourceT));

        // Parse and append the resource
        RT_SAFE_RET(RT_FrostyParseResource(buf, resource));
        RT_SAFE_RET(RT_ArrayAppend(arr, resource));
    }

    if (outResources != NULL)
    {
        *outResources = arr;
    }

    return RT_SUCCESS;
}

void RT_FrostyDestroyResource(RTFrostyResourceT* resource)
{
    if (resource == NULL)
    {
        return;
    }

    RT_SafeFree(resource->name);
    RT_SafeFree(resource->userData);

    free(resource);
}

static uint32_t _SwapEndiannessU32(uint32_t val)
{
    return ((val >> 24) & 0xff) | ((val << 8) & 0xff0000) | ((val >> 8) & 0xff00) | ((val << 24) & 0xff000000);
}

static int32_t _SwapEndiannessS32(int32_t val)
{
    int32_t result = 0;
    result |= (val & 0x000000FF) << 24;
    result |= (val & 0x0000FF00) << 8;
    result |= (val & 0x00FF0000) >> 8;
    result |= (val & 0xFF000000) >> 24;
    return result;
}

static uint16_t _SwapEndiannessU16(uint16_t val)
{
    uint16_t result = 0;
    result |= (val & 0x00FF) << 8;
    result |= (val & 0xFF00) >> 8;
    return result;
}

RTErrorT RT_FrostyDecompressEbx(const char* buf, size_t size, char** outBuf, size_t* outSize)
{
    char* writer = calloc(0, sizeof(uint8_t));
    size_t total = 0;

    while (size > 0)
    {
        // Read chunk meta
        uint32_t bufferSize = RT_READ_BUF(uint32_t, &buf);
        bufferSize = _SwapEndiannessU32(bufferSize);
        if (bufferSize > 0x10000)
        {
            RT_LogF("Failing to read resource of size %d (%d)", bufferSize, size);
            return RT_ERROR_RESOURCE_TOO_BIG;
        }

        uint16_t compressCode = RT_READ_BUF(uint16_t, &buf);
        RT_UNUSED(compressCode);

        uint16_t compressSize = RT_READ_BUF(uint16_t, &buf);
        compressSize = _SwapEndiannessU16(compressSize);

        // Extend the buffer
        char* newBuf = realloc(writer, total + bufferSize);
        if (newBuf == NULL)
        {
            free(writer);
            return RT_ERROR_ALLOCATION_FAILED;
        }
        writer = newBuf;

        // Decompress
        size_t err = ZSTD_decompress(writer + total, bufferSize, buf, compressSize);
        if (ZSTD_isError(err))
        {
            RT_LogF("Decompression failed: %s", ZSTD_getErrorName(err));
            return RT_ERROR_DECOMPRESSION_FAILED;
        }

        buf += compressSize;

        size -= (size_t)compressSize + 8;
        total += bufferSize;
    }

    if (outBuf != NULL)
    {
        *outBuf = writer;
    }

    if (outSize != NULL)
    {
        *outSize = total;
    }

    return RT_SUCCESS;
}
