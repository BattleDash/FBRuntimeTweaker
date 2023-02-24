// Copyright Sean Kahler. All Rights Reserved.

#include "parse_common.h"

#include <stdlib.h>
#include <string.h>

RTErrorT RT_ReadSizedString(const char** buf, char** outStr)
{
    // Read and allocate string size
    size_t strSize = RT_READ_BUF(uint8_t, buf);
    char* str = malloc(strSize + 1);
    if (str == NULL)
    {
        return RT_ERROR_ALLOCATION_FAILED;
    }

    // Copy string into the new buffer
    memcpy(str, *buf, strSize);
    *buf += strSize;

    // Null terminate
    str[strSize] = 0;

    if (outStr != NULL)
    {
        *outStr = str;
    }

    return RT_SUCCESS;
}

RTErrorT RT_ReadTerminatedString(const char** buf, char** outStr)
{
    // Read and allocate string size
    size_t strSize = strlen(*buf) + 1;
    char* str = malloc(strSize);
    if (str == NULL)
    {
        return RT_ERROR_ALLOCATION_FAILED;
    }

    // Copy string into the new buffer
    memcpy(str, *buf, strSize);
    *buf += strSize;

    if (outStr != NULL)
    {
        *outStr = str;
    }

    return RT_SUCCESS;
}

void RT_ReadSha1(const char** buf, char* outBuf)
{
    int32_t sha1TypeSize = 20 * sizeof(uint8_t);
    memcpy(outBuf, *buf, sha1TypeSize);
    *buf += sha1TypeSize;
}
