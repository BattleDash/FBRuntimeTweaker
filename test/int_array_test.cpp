#include <gtest/gtest.h>

#include "array.h"

// Fixture for wrapping array creation
class IntArrayTest : public testing::Test
{
protected:
    void SetUp()
    {
        EXPECT_EQ(RT_ArrayCreate(sizeof(uintptr_t), &arr), RT_SUCCESS);
    }

    void TearDown()
    {
        RT_ArrayDestroy(arr);
    }

    RTArrayT* arr;
};

// Append a value to the array
TEST_F(IntArrayTest, ArrayAppend)
{
    EXPECT_EQ(RT_ArrayAppend(arr, (void*) 102478), RT_SUCCESS);

    EXPECT_EQ(RT_ArrayGetSize(arr), 1);
    EXPECT_EQ((uint32_t) RT_ArrayGetElement(arr, 0), 102478);
}
