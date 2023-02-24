#include <gtest/gtest.h>

#include "frosty_mod.h"

#ifndef TEST_FBMOD_FILE
#define TEST_FBMOD_FILE "test_data/Test.fbmod"
#endif

// Fixture for wrapping mod loading
class FrostyTest : public testing::Test
{
protected:
	void SetUp()
	{
		// Load the file into memory
		char* data = nullptr;
		size_t size;
		EXPECT_EQ(RT_LoadFile(TEST_FBMOD_FILE, &data, &size), RT_SUCCESS);

		// Load the mod
		EXPECT_EQ(RT_FrostyLoadMod(data, size, &mod), RT_SUCCESS);

		// No longer needed
		RT_SafeFree(data);
	}

	void TearDown()
	{
		RT_FrostyDestroyMod(mod);
	}

	RTFrostyModT* mod;
};

// Verify some metadata about the mod
TEST_F(FrostyTest, ModMetadata)
{
	EXPECT_STREQ(mod->profile, "starwarsbattlefrontii");
	EXPECT_STREQ(mod->details.title, "Test");
	EXPECT_STREQ(mod->details.author, "BattleDash");
}

// Verify that the resources are properly loaded
TEST_F(FrostyTest, ModResources)
{
	size_t size = RT_ArrayGetSize(mod->resources);
	EXPECT_EQ(size, 11);

	for (int i = 0; i < size; ++i)
	{
		RTFrostyResourceT* resource = (RTFrostyResourceT*)RT_ArrayGetElement(mod->resources, i);
		RT_LogF("Resource name: %s", resource->name);

		if (!strcmp(resource->name, "gameplay/kits/hero/bobafett/prefabs/pf_hero_bobafett_jetpack"))
		{
			char* buffer = nullptr;
			size_t size = 0;
			EXPECT_EQ(RT_FrostyReadModResource(mod, resource, &buffer, &size), RT_SUCCESS);

			RT_SaveFile("test_data/Test.ebx", buffer, size);
		}
	}
}
