#include <gtest/gtest.h>
#include "../mundijson.hpp"

#include <string>

using namespace mundi;

TEST(json_array, empty)
{
  std::string jstr{ "[]" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_array());
  EXPECT_EQ(val->string_value(), "[]");
}

TEST(json_array, homogeny)
{
  std::string jstr{ "[true, false]"};
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_array());
  EXPECT_EQ(val->string_value(), "[true, false]");

  jstr.assign("[\"abc\", \"物质\"]");
  val = parse_string(jstr);

  EXPECT_TRUE(val->is_array());
  EXPECT_EQ(val->string_value(), "[\"abc\", \"物质\"]");

  jstr.assign("[123, -456, 0.789]");
  val = parse_string(jstr);

  EXPECT_TRUE(val->is_array());
  EXPECT_EQ(val->string_value(), "[123, -456, 0.789]");
}

TEST(json_array, mix)
{
  std::string jstr{ "[true, 123, 0.123, -1.0, \"abc\"]"};
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_array());
  EXPECT_EQ(val->string_value(), "[true, 123, 0.123, -1.0, \"abc\"]");
}
