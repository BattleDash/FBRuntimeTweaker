// Copyright Sean Kahler. All Rights Reserved.

#pragma once

#include "util.h"

EXTERN_C_BLOCK_START

// Read a string with a size byte prefix
RTErrorT RT_ReadSizedString(const char** buf, char** outStr);

// Read a null-terminated string
RTErrorT RT_ReadTerminatedString(const char** buf, char** outStr);

// Read a 20-byte SHA1 hash without allocating new memory
void RT_ReadSha1(const char** buf, char* outBuf);

EXTERN_C_BLOCK_END