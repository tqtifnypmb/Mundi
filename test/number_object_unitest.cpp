#include <gtest/gtest.h>
#include "../src/mundijson.hpp"

#include <string>

using namespace mundi;

TEST(json_number, int_1)
{
  std::string jstr{ "0" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_TRUE(val->is_int());
  EXPECT_EQ(val->int_value(), 0);
  EXPECT_EQ(val->double_value(), 0);
  EXPECT_EQ(val->string_value(), "0");
}

TEST(json_number, int_2)
{
  std::string jstr{"  123  "};
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_TRUE(val->is_int());
  EXPECT_EQ(val->int_value(), 123);
  EXPECT_EQ(val->double_value(), 123);
  EXPECT_EQ(val->string_value(), "123");
}

TEST(json_number, int_3)
{
  std::string jstr{"-1234567890"};
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_TRUE(val->is_int());
  EXPECT_EQ(val->int_value(), -1234567890);
  EXPECT_EQ(val->string_value(), "-1234567890");
}

TEST(json_number, double_1)
{
  std::string jstr{ "0.0" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_TRUE(val->is_double());
  EXPECT_EQ(val->int_value(), 0);
  EXPECT_EQ(val->double_value(), 0);
  EXPECT_EQ(val->string_value(), "0.0");
}

TEST(json_number, double_2)
{
  std::string jstr{ "0.789" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_TRUE(val->is_double());
  EXPECT_EQ(val->double_value(), 0.789);
  EXPECT_EQ(val->int_value(), 0);
  EXPECT_EQ(val->string_value(), "0.789");

  jstr.assign("   1.789    ");
  val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_TRUE(val->is_double());
  EXPECT_EQ(val->double_value(), 1.789);
  EXPECT_EQ(val->int_value(), 1);
  EXPECT_EQ(val->string_value(), "1.789");
}

TEST(json_number, double_3)
{
  std::string jstr{"-0.789"};
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_TRUE(val->is_double());
  EXPECT_EQ(val->double_value(), -0.789);
  EXPECT_EQ(val->int_value(), 0);

  jstr.assign("-1.789");
  val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_TRUE(val->is_double());
  EXPECT_EQ(val->double_value(), -1.789);
  EXPECT_EQ(val->int_value(), -1);
}

TEST(json_number, exp)
{
  std::string jstr{ "1.1e-10" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_EQ(val->double_value(), 1.1e-10);
}

TEST(json_number, exp_2)
{
  std::string jstr{ "1.1e2" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_EQ(val->double_value(), 1.1e2);
}

TEST(object_type_detect, space)
{
  std::string jstr{ "\n\n\n\n123\n\n\n\t" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_EQ(val->int_value(), 123);
}

TEST(object_type_detect, space_2)
{
  std::string jstr{ "\n\n\n\n1.23\n\n\n\t" };
  auto val = parse_string(jstr);

  EXPECT_TRUE(val->is_number());
  EXPECT_EQ(val->double_value(), 1.23);
}
