// Copyright Sean Kahler. All Rights Reserved.

#include "entry_point.h"

#include <MinHook.h>

#define RT_OFFSET_LOADEBXFROMMEM (LPVOID) 0x14020F070
#define RT_OFFSET_FINDEBXFROMGUID (LPVOID) 0x140209310

void _OpenConsole(RTRuntimeTweakerT* rt)
{
	// Open a console
	AllocConsole();
	freopen_s(&rt->consoleFile, "CONOUT$", "w", stdout);

	// ANSI Colors
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode;
	GetConsoleMode(stdoutHandle, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(stdoutHandle, dwMode);
}

void RT_Eject(RTRuntimeTweakerT* rt)
{
	RT_LogF("Goodbye!");

	if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK
		|| MH_RemoveHook(MH_ALL_HOOKS) != MH_OK
		|| MH_Uninitialize() != MH_OK)
	{
		RT_LogF("MinHook failed to uninitialize?");
	}

	HMODULE hModule = NULL;

	if (rt != NULL)
	{
		hModule = rt->nativeModule;

		fclose(rt->consoleFile);
		FreeConsole();

		RT_HookManDestroy(rt->hookMan);
		RT_Destroy(rt);
	}
	else
	{
		hModule = GetModuleHandle(NULL);
	}

	FreeLibraryAndExitThread(hModule, 0);
}

// This is temporary, I just needed a fast way for the hooks to access the RT instance,
// eventually I'd like to make a hooking library that allows for context parameters.
static RTRuntimeTweakerT* rt;

RT_DECLARE_HOOK(bool, LoadEbxFromMem, void* inst, const char* name, const char* inBuffer, uint32_t inSize,
	void* a5, void* a6, void** ebxPartitionOut)
{
	size_t modCount = RT_ArrayGetSize(rt->mods);
	for (int i = 0; i < modCount; ++i)
	{
		RTFrostyModT* mod = (RTFrostyModT*)RT_ArrayGetElement(rt->mods, i);

		size_t resourceCount = RT_ArrayGetSize(mod->resources);
		for (int i = 0; i < resourceCount; ++i)
		{
			RTFrostyResourceT* resource = (RTFrostyResourceT*)RT_ArrayGetElement(mod->resources, i);

			if (!strcmp(name, resource->name))
			{
				// TODO Need to find something to hook to
				// properly free these loaded mod resources

				char* buffer = NULL;
				size_t size = 0;
				RT_FrostyReadModResource(mod, resource, &buffer, &size);

				inBuffer = buffer;
				inSize = (uint32_t) size;

				RT_LogF("Loading %s on %p", name, inst);
			}
		}
	}

	return fpLoadEbxFromMem(inst, name, inBuffer, inSize, a5, a6, ebxPartitionOut);
}

RT_DECLARE_HOOK(void*, FindEbxFromGuid, void* inst, const void* guid)
{
	// TODO Load duplicated assets, has some unsolved issues with entity creation
	return fpFindEbxFromGuid(inst, guid);
}

DWORD WINAPI _EntryPoint(HMODULE hModule)
{
	RT_LogF("Starting Runtime Tweaker");

	if (RT_Create(&rt) != RT_SUCCESS)
	{
		RT_LogF("Failed to create RuntimeTweaker instance, bailing out");
		RT_Eject(NULL);
		return 0;
	}

	_OpenConsole(rt);

	if (MH_Initialize() != MH_OK)
	{
		RT_LogF("Failed to initialize MinHook");
		RT_Eject(rt);
		return 0;
	}

	if (RT_HookManCreate(&rt->hookMan) != RT_SUCCESS)
	{
		RT_LogF("Failed to initialize HookManager");
		RT_Eject(rt);
		return 0;
	}

	RT_HookManCreateHook(rt->hookMan, RT_OFFSET_LOADEBXFROMMEM, _LoadEbxFromMemHook, &fpLoadEbxFromMem);
	RT_HookManCreateHook(rt->hookMan, RT_OFFSET_FINDEBXFROMGUID, _FindEbxFromGuidHook, &fpFindEbxFromGuid);

	// TODO Add a console command or config file for loading mods
	char* data = NULL;
	size_t size;
	if (RT_LoadFile("E:\\A-Programming\\C++\\FBRuntimeTweaker\\test\\data\\Test.fbmod", &data, &size) != RT_SUCCESS)
	{
		RT_LogF("Failed to load mod");
		RT_Eject(rt);
		return 0;
	}

	// Load the mod
	RTFrostyModT* mod;
	RT_FrostyLoadMod(data, size, &mod);

	// No longer needed
	RT_SafeFree(data);

	// Should we really be doing this manually?
	// I almost want an RT_LoadMod(rt, filename) or something
	RT_ArrayAppend(rt->mods, mod);

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, const DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        CloseHandle(CreateThread(NULL, 0, _EntryPoint, hInstance, 0, 0));
    }
    return TRUE;
}