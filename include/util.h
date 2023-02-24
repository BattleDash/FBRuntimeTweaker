// Copyright Sean Kahler. All Rights Reserved.

#pragma once

#include "error.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#define EXTERN_C_BLOCK_START extern "C" {
#define EXTERN_C_BLOCK_END   }
#else
#define EXTERN_C_BLOCK_START
#define EXTERN_C_BLOCK_END
#endif

#define RT_READ_BUF_OFF(type, buf, offset) (*(type*) (buf + offset)); offset += sizeof(type)
#define RT_READ_BUF(type, buf) *(type*) *buf; *buf += sizeof(type)
#define RT_SAFE_RET(err) if (err != RT_SUCCESS) return err
#define RT_UNUSED(x) (void)x

EXTERN_C_BLOCK_START

// Log something to the console
void RT_LogF(const char* format, ...);

// Check if a file exists
bool RT_FileExists(const char* file);

// Load a file into memory
RTErrorT RT_LoadFile(const char* file, char** outBuf, size_t* outSize);

// Save a buffer to disk
RTErrorT RT_SaveFile(const char* file, const char* buffer, size_t size);

// free() that accepts null values
void RT_SafeFree(void* mem);

EXTERN_C_BLOCK_END