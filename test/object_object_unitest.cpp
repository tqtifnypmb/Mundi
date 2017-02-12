#include <gtest/gtest.h>
#include "../src/mundijson.hpp"

#include <string>

using namespace mundi;

TEST(json_object, empty)
{
  std::string jstr{ "{}" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_object());
  EXPECT_EQ(val->string_value(), "{}");
}

TEST(json_object, basic)
{
  std::string jstr{ "{\"number\": 123, \"string\": \"abc\", \"boolean\": true}" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_object());
  EXPECT_EQ((*val)["number"].int_value(), 123);
  EXPECT_EQ((*val)[{"string"}].string_value(), "abc");
  EXPECT_EQ((*val)[{"boolean"}].boolean_value(), true);
}

TEST(json_object, basic2)
{
  std::string jstr{ "{\"key\": {\"key1\": {\"key2\": \"value\"}}}"};
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_object());
  EXPECT_TRUE((*val)["key"].is_object());
  EXPECT_TRUE((*val)["key"]["key1"].is_object());
  EXPECT_FALSE((*val)["key"]["key1"]["key2"].is_object());
  EXPECT_TRUE((*val)["key"]["key1"]["key2"].is_string());
  EXPECT_EQ((*val)["key"]["key1"]["key2"].string_value(), "value");
}

TEST(json_object, basic3)
{
  std::string jstr{ "{\"key\": {}}" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_object());
  EXPECT_TRUE((*val)["key"].is_object());
}

TEST(object_type_detect, space)
{
  std::string jstr { "      \n\n\n\n{ \t\t\n\n \"key\": \n\n\n\"value\"\n\n\n\n, \t\n \"key2\": true\n,\"key3\": \n\n{\n\n\"key4\"\n:\nfalse}}\t\n" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_object());
  EXPECT_EQ((*val)["key"].string_value(), "value");
  EXPECT_EQ((*val)["key2"].boolean_value(), true);
  EXPECT_TRUE((*val)["key3"].is_object());
  EXPECT_EQ((*val)["key3"]["key4"].boolean_value(), false);
}
