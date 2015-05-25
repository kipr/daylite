#include "gtest/gtest.h"
#include "daylite/option.hpp"
#include <unistd.h>

using namespace daylite;

TEST(option, some)
{
  option<uint32_t> t = some(123U);
  EXPECT_TRUE(t.some());
  EXPECT_FALSE(t.none());
  EXPECT_EQ(t.unwrap(), 123);
  EXPECT_EQ(t.or_else(321), 123);
}

TEST(option, none)
{
  option<uint32_t> t = none<uint32_t>();
  EXPECT_FALSE(t.some());
  EXPECT_TRUE(t.none());
  EXPECT_EQ(t.or_else(321), 321);
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}