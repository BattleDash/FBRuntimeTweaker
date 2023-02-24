#include <gtest/gtest.h>

#include "array.h"

// Fixture for wrapping array creation
class ArrayTest : public testing::Test
{
protected:
    void SetUp()
    {
        EXPECT_EQ(RT_ArrayCreate(sizeof(char*), &arr), RT_SUCCESS);
    }

    void TearDown()
    {
        RT_ArrayDestroy(arr);
    }

    RTArrayT* arr;
};

// Append a value to the array
TEST_F(ArrayTest, ArrayAppend)
{
    char c = 'a';
    EXPECT_EQ(RT_ArrayAppend(arr, &c), RT_SUCCESS);

    EXPECT_EQ(RT_ArrayGetSize(arr), 1);
    EXPECT_EQ(RT_ArrayGetElement(arr, 0), &c);
}

// Append a value to the array
TEST_F(ArrayTest, ArrayRemove)
{
    char c = 'a';
    EXPECT_EQ(RT_ArrayAppend(arr, &c), RT_SUCCESS);
    EXPECT_EQ(RT_ArrayGetSize(arr), 1);

    EXPECT_EQ(RT_ArrayRemove(arr, 0), RT_SUCCESS);
    EXPECT_EQ(RT_ArrayGetSize(arr), 0);
}