// Copyright Sean Kahler. All Rights Reserved.

#pragma once

#include "util.h"

// Create a typedef, instance, and function declaration
#define RT_DECLARE_HOOK(retType, name, ...) \
    typedef retType(*RTFunc##name##T)(##__VA_ARGS__##); \
    RTFunc##name##T fp##name = NULL; \
    retType _##name##Hook(##__VA_ARGS__##)

typedef struct RTHookManagerT RTHookManagerT;

EXTERN_C_BLOCK_START

// Create a hook manager
RTErrorT RT_HookManCreate(RTHookManagerT** outMan);

// Create a hook
RTErrorT RT_HookManCreateHook(RTHookManagerT* man, void* target, void* detour, void* trampoline);

// Destroy a hook manager
void RT_HookManDestroy(RTHookManagerT* man);

EXTERN_C_BLOCK_END