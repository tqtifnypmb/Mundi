#include <gtest/gtest.h>
#include "../src/mundijson.hpp"

#include <string>

using namespace mundi;

TEST(json_boolean, basic_true)
{
  std::string jstr{ "true" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_bool());
  EXPECT_TRUE(val->boolean_value());
  EXPECT_EQ(val->string_value(), "true");
}

TEST(json_boolean, basic_false)
{
  std::string jstr{ "false" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_bool());
  EXPECT_FALSE(val->boolean_value());
  EXPECT_EQ(val->string_value(), "false");
}
