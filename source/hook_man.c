// Copyright Sean Kahler. All Rights Reserved.

#include "hook_man.h"

#include "array.h"

#include <MinHook.h>
#include <stdlib.h>

typedef struct
{
    void* target;
    void* detour;
} RTHookT;

struct RTHookManagerT
{
    RTArrayT* hooks;
};

RTErrorT RT_HookManCreate(RTHookManagerT** outMan)
{
    // Allocate new instance
    RTHookManagerT* manager = malloc(sizeof(RTHookManagerT));
    if (manager == NULL)
    {
        return RT_ERROR_ALLOCATION_FAILED;
    }
    memset(manager, 0, sizeof(RTHookManagerT));

    // Initialize the hooks array
    RT_SAFE_RET(RT_ArrayCreate(sizeof(RTHookT*), &manager->hooks));

    if (outMan != NULL)
    {
        *outMan = manager;
    }

    return RT_SUCCESS;
}

RTErrorT RT_HookManCreateHook(RTHookManagerT* man, void* target, void* detour, void* trampoline)
{
    RTHookT* hook = malloc(sizeof(RTHookT));
    if (hook == NULL)
    {
        return RT_ERROR_ALLOCATION_FAILED;
    }

    hook->target = target;
    hook->detour = detour;

    MH_STATUS mhErr = MH_CreateHook(target, detour, trampoline);
    if (mhErr != MH_OK && mhErr != MH_ERROR_ALREADY_CREATED)
    {
        free(hook);
        return RT_ERROR_HOOK_CREATION_FAILED;
    }

    mhErr = MH_EnableHook(target);
    if (mhErr != MH_OK && mhErr != MH_ERROR_ENABLED)
    {
        MH_RemoveHook(target);

        free(hook);
        return RT_ERROR_HOOK_CREATION_FAILED;
    }

    RTErrorT arrErr = RT_ArrayAppend(man->hooks, hook);
    if (arrErr != RT_SUCCESS)
    {
        free(hook);
        return arrErr;
    }

    return RT_ERROR_HOOK_CREATION_FAILED;
}

void RT_HookManDestroy(RTHookManagerT* man)
{
    if (man == NULL)
    {
        return;
    }

    free(man);
}
