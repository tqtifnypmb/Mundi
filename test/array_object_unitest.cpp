#include <gtest/gtest.h>
#include "../src/mundijson.hpp"

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
  EXPECT_TRUE((*val)[0].is_bool());
  EXPECT_EQ((*val)[0].boolean_value(), true);
}

TEST(object_type_detect, space)
{
  std::string jstr{ "\n\n\n\n[\n\n\ntrue\n\n,123\n\n,\n\n-1.0\n,\"abc\"\n\n]\n\n\n\n" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_array());
  EXPECT_EQ((*val)[0].boolean_value(), true);
  EXPECT_EQ((*val)[1].int_value(), 123);
  EXPECT_EQ((*val)[2].double_value(), -1.0);
  EXPECT_EQ((*val)[3].string_value(), "abc");
}

TEST(object_type_detect, space_2)
{
  std::string jstr{ "\n\n\n\n[\n\n\n]\n\n\n\n" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_array());
}
