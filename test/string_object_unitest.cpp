#include <gtest/gtest.h>
#include "../mundijson.hpp"

#include <string>

using namespace mundi;

TEST(json_string, empty)
{
  std::string jstr{ "\"\"" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_string());
  EXPECT_EQ(val->string_value(), "");
}

TEST(json_string, escape)
{
  std::string jstr{ "\"abc\tdef\nghi\\n\\\"\"" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_string());
  EXPECT_EQ(val->string_value(), "abc\tdef\nghi\n\"");
}

TEST(json_string, escape_unicode)
{
  // TODO
}

TEST(json_string, unicode)
{
  std::string jstr{ "\"饮食\"" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_string());
  EXPECT_EQ(val->string_value(), "饮食");
}

TEST(json_string, space)
{
  std::string jstr{ "\"  abc  def \""};
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_string());
  EXPECT_EQ(val->string_value(), "  abc  def ");
}

TEST(json_string, fake_type)
{
  std::string jstr{ "\"true\"" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_string());
  EXPECT_EQ(val->string_value(), "true");

  jstr.assign("\"123\"");
  val = parse_string(jstr);

  EXPECT_TRUE(val->is_string());
  EXPECT_EQ(val->string_value(), "123");
}

TEST(json_string, mix)
{
  std::string jstr{ "\"abcdef饮食123\"" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_string());
  EXPECT_EQ(val->string_value(), "abcdef饮食123");
}
