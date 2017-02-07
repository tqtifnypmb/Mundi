#include "../mundijson.hpp"
#include <string>
#include <iostream>

using namespace mundi;

void simple_test(const std::string& jstr) {
  auto json = parse_string(jstr);

  std::cout<<jstr<<std::endl;
  std::cout<<json->string_value()<<std::endl;
}

int main()
{
  simple_test({"true"});
  simple_test({"TrUe"});
  simple_test({"FaLSe"});
  simple_test({"123"});
  simple_test({ "[true, false, 1.234, 0.13e23, -1.2, -1]" });
  simple_test({ "[true, false, [1.23, 4, 6, 10], []]" });
  simple_test({ "[]" });
  simple_test({ "{}" });
  simple_test({ "" });
  simple_test({ "\"abcdef\"" });
  simple_test({ "{\"key\": true, \"key2\": false, \"key3\": 132, \"key4\": {}}" });
  simple_test({ "\"abc\tdef\n\""});
}
