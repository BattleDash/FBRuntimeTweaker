// Copyright Sean Kahler. All Rights Reserved.

#include "runtime_tweaker.h"

#include <stdlib.h>

RTErrorT RT_Create(RTRuntimeTweakerT** outRT)
{
	// Allocate new instance
	RTRuntimeTweakerT* rt = malloc(sizeof(RTRuntimeTweakerT));
	if (rt == NULL)
	{
		return RT_ERROR_ALLOCATION_FAILED;
	}

	RT_SAFE_RET(RT_ArrayCreate(sizeof(RTFrostyModT*), &rt->mods));

	if (outRT != NULL)
	{
		*outRT = rt;
	}

	return RT_SUCCESS;
}

void RT_Destroy(RTRuntimeTweakerT* rt)
{
	free(rt);
}
