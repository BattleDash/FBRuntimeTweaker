#include <gtest/gtest.h>

#include "util.h"

// Log a test value
TEST(LogTest, Log)
{
	RT_LogF("Test %d", 1);
}