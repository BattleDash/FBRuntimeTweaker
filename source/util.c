// Copyright Sean Kahler. All Rights Reserved.

#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32 
#include <io.h>
#define access _access
#else
#include <unistd.h>
#endif

void RT_LogF(const char* format, ...)
{
    va_list fmtList;
    va_start(fmtList, format);

    char outStr[4096];
    vsprintf(outStr, format, fmtList);
    va_end(fmtList);

    printf("[RuntimeTweaker] %s\n", outStr);
}

bool RT_FileExists(const char* file)
{
    return access(file, 0) == 0;
}

RTErrorT RT_LoadFile(const char* file, char** outBuf, size_t* outSize)
{
    FILE* fp = fopen(file, "rb");
    if (fp == NULL) {
        return RT_ERROR_INVALID_FILE;
    }

    // get the size of the file
    fseek(fp, 0L, SEEK_END);
    const size_t size = ftell(fp);

    // set the file position to the beginning
    rewind(fp);

    // allocate a buffer large enough to hold the file's contents
    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(fp);
        return RT_ERROR_ALLOCATION_FAILED;
    }

    // read the file into the buffer
    if (fread(buffer, size, 1, fp) != 1) {
        free(buffer);
        fclose(fp);
        return RT_ERROR_READ_FAILED;
    }

    buffer[size] = 0;
    fclose(fp);

    if (outBuf != NULL)
    {
        *outBuf = buffer;
    }

    if (outSize != NULL)
    {
        *outSize = size;
    }

    return RT_SUCCESS;
}

RTErrorT RT_SaveFile(const char* file, const char* buffer, size_t size)
{
	FILE* fp = fopen(file, "wb");
	if (fp == NULL)
	{
        return RT_ERROR_INVALID_FILE;
	}

	fwrite(buffer, size, 1, fp);
	fclose(fp);

    return RT_SUCCESS;
}

void RT_SafeFree(void* mem)
{
	if (mem != NULL)
	{
		free(mem);
	}
}
