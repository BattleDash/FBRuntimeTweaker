// Copyright Sean Kahler. All Rights Reserved.

#pragma once

#include "array.h"
#include "frosty_mod.h"
#include "hook_man.h"

#include <Windows.h>
#include <stdio.h>

typedef struct
{
	HMODULE nativeModule;
	FILE* consoleFile;

	RTHookManagerT* hookMan;

	RTArrayT* mods;
} RTRuntimeTweakerT;

EXTERN_C_BLOCK_START

// Create a new RuntimeTweaker instance
RTErrorT RT_Create(RTRuntimeTweakerT** outRT);

// Destroy a RuntimeTweaker instance
void RT_Destroy(RTRuntimeTweakerT* rt);

EXTERN_C_BLOCK_END