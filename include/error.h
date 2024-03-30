// Copyright Sean Kahler. All Rights Reserved.

#pragma once

#include <stdint.h>

typedef int32_t RTErrorT;

#define RT_SUCCESS 0

// Error Groups
#define RT_ERROR_GENERAL 0
#define RT_ERROR_FROSTY (1<<16)
#define RT_ERROR_HOOK   (2<<16)

// General Errors
#define RT_ERROR_ALLOCATION_FAILED	  (RT_ERROR_GENERAL + 0)
#define RT_ERROR_READ_FAILED	      (RT_ERROR_GENERAL + 1)
#define RT_ERROR_INVALID_ARGUMENT     (RT_ERROR_GENERAL + 2)
#define RT_ERROR_INVALID_FILE		  (RT_ERROR_GENERAL + 3)
#define RT_ERROR_DECOMPRESSION_FAILED (RT_ERROR_GENERAL + 4)

// Frosty Errors
#define RT_ERROR_INVALID_MOD_HEADER (RT_ERROR_FROSTY + 0)
#define RT_ERROR_RESOURCE_TOO_BIG	(RT_ERROR_FROSTY + 1)

// Hook Errors
#define RT_ERROR_HOOK_CREATION_FAILED (RT_ERROR_HOOK + 0)