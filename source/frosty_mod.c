// Copyright Sean Kahler. All Rights Reserved.

#include "frosty_mod.h"

#include "parse_common.h"

#include <stdlib.h>
#include <string.h>

const uint64_t kFrostyMagic = 0x01005954534F5246;

const uint64_t kFrostyLowestVersion = 4;
const uint64_t kFrostyHighestVersion = 5;

RTErrorT _ParseModHeader(const char** buf, RTFrostyModT* mod)
{
    // Read magic values & data header
    mod->magic = RT_READ_BUF(uint64_t, buf);
    mod->version = RT_READ_BUF(uint32_t, buf);

    if (mod->magic != kFrostyMagic || mod->version < kFrostyLowestVersion || mod->version > kFrostyHighestVersion)
    {
        return RT_ERROR_INVALID_MOD_HEADER;
    }

    mod->dataOffset = RT_READ_BUF(int64_t, buf);
    mod->dataCount = RT_READ_BUF(int32_t, buf);

    // Read profile string
    RT_SAFE_RET(RT_ReadSizedString(buf, &mod->profile));

    // Read game version
    mod->gameVersion = RT_READ_BUF(int32_t, buf);

    return RT_SUCCESS;
}

RTErrorT _ParseModDetails(const char** buf, FrostyModDetailsT* details, uint32_t version)
{
    RT_SAFE_RET(RT_ReadTerminatedString(buf, &details->title));
    RT_SAFE_RET(RT_ReadTerminatedString(buf, &details->author));
    RT_SAFE_RET(RT_ReadTerminatedString(buf, &details->category));
    RT_SAFE_RET(RT_ReadTerminatedString(buf, &details->version));
    RT_SAFE_RET(RT_ReadTerminatedString(buf, &details->description));

    if (version >= 5)
    {
        RT_SAFE_RET(RT_ReadTerminatedString(buf, &details->link));
    }

    return RT_SUCCESS;
}

RTErrorT RT_FrostyLoadMod(const char* buf, size_t size, RTFrostyModT** outMod)
{
    if (buf == NULL)
    {
        return RT_ERROR_INVALID_ARGUMENT;
    }

    // Allocate new mod
    RTFrostyModT* mod = malloc(sizeof(RTFrostyModT));
    if (mod == NULL)
    {
        return RT_ERROR_ALLOCATION_FAILED;
    }

    // Zero-out to avoid panics when freeing after an error
    memset(mod, 0, sizeof(RTFrostyModT));

    // Copy original data for resource loading
    mod->data = malloc(size);
    if (mod->data == NULL)
    {
        return RT_ERROR_ALLOCATION_FAILED;
    }

    memcpy(mod->data, buf, size);

    RTErrorT parseErr = _ParseModHeader(&buf, mod);
    if (parseErr != RT_SUCCESS)
    {
        RT_FrostyDestroyMod(mod);
        return parseErr;
    }

    parseErr = _ParseModDetails(&buf, &mod->details, mod->version);
    if (parseErr != RT_SUCCESS)
    {
        RT_FrostyDestroyMod(mod);
        return parseErr;
    }

    RT_LogF("Parsing mod '%s' by '%s'", mod->details.title, mod->details.author);

    parseErr = RT_FrostyParseResources(&buf, &mod->resources);
    if (parseErr != RT_SUCCESS)
    {
        RT_FrostyDestroyMod(mod);
        return parseErr;
    }

    if (outMod != NULL)
    {
        *outMod = mod;
        return RT_SUCCESS;
    }

    RT_FrostyDestroyMod(mod);
    return RT_SUCCESS;
}

RTErrorT RT_FrostyReadModResource(RTFrostyModT* mod, RTFrostyResourceT* resource, char** outBuf, size_t* outSize)
{
    if (resource->resourceIndex == -1)
    {
        return RT_ERROR_INVALID_ARGUMENT;
    }

    int64_t offset = mod->dataOffset + (resource->resourceIndex * 16);

    int64_t dataOffset = RT_READ_BUF_OFF(int64_t, mod->data, offset);
    int64_t dataSize = RT_READ_BUF_OFF(int64_t, mod->data, offset);

    offset = mod->dataOffset + (mod->dataCount * 16) + dataOffset;

    // Intercept and decompress if EBX
    if ((resource->type == FrostyResourceType_Ebx || resource->type == FrostyResourceType_Res ||
            resource->type == FrostyResourceType_Chunk) &&
        resource->handleHash == 0)
    {
        return RT_FrostyDecompressEbx(mod->data + offset, dataSize, outBuf, outSize);
    }

    char* buffer = malloc(dataSize);
    if (buffer == NULL)
    {
        return RT_ERROR_ALLOCATION_FAILED;
    }

    // Copy the resource data to the new buffer
    memcpy(buffer, mod->data + offset, dataSize);

    if (outBuf != NULL)
    {
        *outBuf = buffer;
    }

    if (outSize != NULL)
    {
        *outSize = dataSize;
    }

    return RT_SUCCESS;
}

void _DestroyModDetails(FrostyModDetailsT details)
{
    RT_SafeFree(details.title);
    RT_SafeFree(details.author);
    RT_SafeFree(details.category);
    RT_SafeFree(details.version);
    RT_SafeFree(details.description);
    RT_SafeFree(details.link);
}

void RT_FrostyDestroyMod(RTFrostyModT* mod)
{
    if (mod == NULL)
    {
        return;
    }

    // Free metadata
    _DestroyModDetails(mod->details);

    // Free resources
    if (mod->resources != NULL)
    {
        size_t resourceCount = RT_ArrayGetSize(mod->resources);
        for (size_t i = 0; i < resourceCount; ++i)
        {
            RTFrostyResourceT* resource = RT_ArrayGetElement(mod->resources, i);
            if (resource == NULL)
            {
                continue;
            }

            RT_FrostyDestroyResource(resource);
        }
        RT_ArrayDestroy(mod->resources);
    }

    // Free mod
    RT_SafeFree(mod->data);
    RT_SafeFree(mod->profile);
    free(mod);
}