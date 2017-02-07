#ifndef MUDI_JSON_HPP
#define MUDI_JSON_HPP

#include <unordered_map>
#include <vector>
#include <string>

#include <algorithm>  // max
#include <sstream>  // ostringstream
#include <utility>  // make_pair
#include <memory>   // share_ptr unique_ptr
#include <cctype>
#include <cassert>  // assert
#include <exception>

#define MUDI_NS_BEGIN namespace mundi {
#define MUDI_NS_END   }

#define MUDI_N_NS_BEGIN namespace mundi { namespace {
#define MUDI_N_NS_END   }}

MUDI_N_NS_BEGIN
enum class value_type
{
  array,
  object,
  string,
  number,
  boolean,
  null,
};

MUDI_N_NS_END

// exception

MUDI_N_NS_BEGIN

class not_supported: public std::exception
{
public:
  const char* what() const noexcept override
  {
    return "Not supported operation";
  }
};

class key_not_found: public std::exception
{
public:
  const char* what() const noexcept override
  {
    return "Key not found";
  }
};

class eof: public std::exception
{
public:
  const char* what() const noexcept override
  {
    return "EOF";
  }
};

class unknown_input: public std::exception
{
public:
  const char* what() const noexcept override
  {
    return "Unknown input";
  }
};

MUDI_N_NS_END

// interface

MUDI_NS_BEGIN

class json_value
{
public:
  virtual ~json_value() noexcept = default;

  virtual const json_value& operator[](const std::string& key) const
  {
    throw not_supported();
  }

  virtual const json_value& operator[](unsigned int index) const
  {
    throw not_supported();
  }

  virtual bool boolean_value() const noexcept
  {
    return false;
  }

  virtual std::string string_value() const noexcept
  {
    return "null";
  }

  virtual int int_value() const noexcept
  {
    return 0;
  }

  virtual double double_value() const noexcept
  {
    return 0;
  }

  bool is_bool() const noexcept
  {
    return type() == value_type::boolean;
  }

  bool is_array() const noexcept
  {
    return type() == value_type::array;
  }

  bool is_string() const noexcept
  {
    return type() == value_type::string;
  }

  bool is_number() const noexcept
  {
    return type() == value_type::number;
  }

  virtual bool is_int() const noexcept
  {
    return false;
  }

  virtual bool is_double() const noexcept
  {
    return false;
  }

  bool is_object() const noexcept
  {
    return type() == value_type::object;
  }

  bool is_null() const noexcept
  {
    return type() == value_type::null;
  }

protected:
  virtual value_type type() const noexcept
  {
    return value_type::null;
  }
};

std::unique_ptr<json_value> parse_string(const std::string& str);
std::unique_ptr<json_value> parse_cstring(const char* cstr, unsigned int len);

MUDI_NS_END

// string

MUDI_N_NS_BEGIN

typedef std::shared_ptr<std::string> buffer_ptr;

class json_string: public json_value
{
public:
  json_string(unsigned int pos, unsigned int len, buffer_ptr buf): pos{ pos }, len{ len }, buffer{ buf }
  {}

  json_string(const json_string& rhs): pos{ rhs.pos }, len{ rhs.len }, buffer{ rhs.buffer }
  {}

  json_string& operator=(const json_string& rhs)
  {
    if (&rhs == this) {
      return *this;
    }

    auto tmp = rhs;
    std::swap(*this, tmp);
    return *this;
  }

  json_string(json_string&& rhs): pos{ rhs.pos }, len{ rhs.len }
  {
    buffer = std::move(rhs.buffer);
  }

  json_string& operator=(json_string&& rhs)
  {
    pos = rhs.pos;
    len = rhs.len;
    buffer = std::move(rhs.buffer);

    return *this;
  }

  std::string string_value() const noexcept override
  {
    return buffer->substr(pos, len);
  }

protected:
  value_type type() const noexcept override
  {
    return value_type::string;
  }

private:
  unsigned int pos;
  unsigned int len;
  buffer_ptr buffer;
};

MUDI_N_NS_END

// array

MUDI_N_NS_BEGIN

class json_parser;

class json_array: public json_value
{
  friend json_parser;
public:
  json_array() = default;

  json_array(const json_array& rhs): data{ rhs.data }
  {}

  json_array& operator=(const json_array& rhs)
  {
    if (&rhs == this) {
      return *this;
    }

    auto tmp = rhs;
    std::swap(*this, tmp);
    return *this;
  }

  json_array(json_array&& rhs)
  {
    data = std::move(rhs.data);
  }

  json_array& operator=(json_array&& rhs)
  {
    data = std::move(rhs.data);
    return *this;
  }

  ~json_array()
  {
    for (auto& v : data) {
      delete v;
    }
    data.clear();
  }

  const json_value& operator[](unsigned int index) const override
  {
    return *data.at(index);
  }

  std::string string_value() const noexcept override
  {
    std::ostringstream sout;

    sout<<"[";
    for (const auto& val : data) {
      if (val->is_string()) {
        sout<<"\""<<val->string_value()<<"\""<<", ";
      } else {
        sout<<val->string_value()<<", ";
      }
    }

    std::string cnt = sout.str();
    if (!data.empty()) {
      cnt.erase(cnt.end() - 2, cnt.end());
    }
    cnt.push_back(']');

    return cnt;
  }

protected:
  value_type type() const noexcept override
  {
    return value_type::array;
  }

private:
  void push_back(json_value* val) noexcept
  {
    data.push_back(val);
  }

private:
  std::vector<json_value*> data;
};

MUDI_N_NS_END

// object

MUDI_N_NS_BEGIN

class json_object: public json_value
{
public:
  json_object() = default;

  json_object(const json_object& rhs): data{ rhs.data }
  {}

  json_object& operator=(const json_object& rhs)
  {
    data = rhs.data;
    return *this;
  }

  json_object(json_object&& rhs)
  {
    data = std::move(rhs.data);
  }

  json_object& operator=(json_object&& rhs)
  {
    data = std::move(rhs.data);
    return *this;
  }

  const json_value& operator[](const std::string& key) const override
  {
    auto found = data.find(key);
    if (found == data.cend()) {
      throw key_not_found();
    } else {
      return *(found->second);
    }
  }

  ~json_object()
  {
    for (auto& o : data) {
      delete o.second;
    }
    data.clear();
  }

  bool push_back(const std::string& key, json_value* value) noexcept
  {
    auto ret = data.insert(std::make_pair(key, value));
    return ret.second;
  }

  void remove(const std::string& key)
  {
    auto found = data.find(key);
    if (found != data.end())
      data.erase(found);
  }

  std::string string_value() const noexcept override
  {
    std::ostringstream sout;

    sout<<"{";
    for (const auto& val : data) {
      sout<<val.first<<": "<<val.second->string_value()<<", ";
    }
    std::string cnt = sout.str();

    if (!data.empty()) {
      cnt.erase(cnt.end() - 2, cnt.end());
    }

    cnt.push_back('}');
    return cnt;
  }

protected:
  value_type type() const noexcept override
  {
    return value_type::object;
  }

private:
  std::unordered_map<std::string, json_value*> data;
};

MUDI_N_NS_END

// number

MUDI_N_NS_BEGIN

class json_number: public json_value
{
public:
  json_number(unsigned int pos, unsigned int len, buffer_ptr buf): pos{ pos }, len{ len }, buffer{ buf }
  {}

  json_number(const json_number& rhs): pos{ rhs.pos }, len{ rhs.len }, buffer{ rhs.buffer }
  {}

  ~json_number() = default;

  json_number& operator=(const json_number& rhs)
  {
    if (&rhs == this) {
      return *this;
    }

    pos = rhs.pos;
    len = rhs.len;
    buffer = rhs.buffer;

    return *this;
  }

  json_number(json_number&& rhs): pos{ rhs.pos }, len{ rhs.len }
  {
    buffer = std::move(rhs.buffer);
  }

  json_number& operator=(json_number&& rhs)
  {
    pos = rhs.pos;
    len = rhs.len;
    buffer = std::move(rhs.buffer);

    return *this;
  }

  bool is_int() const noexcept override
  {
    return std::string::npos == string_value().find(".");
  }

  bool is_double() const noexcept override
  {
    return !is_int();
  }

  int int_value() const noexcept override
  {
    return std::stoi(string_value());
  }

  double double_value() const noexcept override
  {
    return std::stod(string_value());
  }

  std::string string_value() const noexcept override
  {
    return buffer->substr(pos, len);
  }

protected:
  value_type type() const noexcept override
  {
    return value_type::number;
  }

private:
  unsigned int pos;
  unsigned int len;
  buffer_ptr buffer;
};

MUDI_N_NS_END

// boolean

MUDI_N_NS_BEGIN

class json_boolean: public json_value
{
public:
  json_boolean(unsigned int pos, unsigned int len, buffer_ptr buf): pos{ pos }, len{ len }, buffer{ buf }
  {}

  json_boolean(const json_boolean& rhs): pos{ rhs.pos }, len{ rhs.len }, buffer{ rhs.buffer }
  {}

  ~json_boolean() = default;

  json_boolean& operator=(const json_boolean& rhs)
  {
    if (&rhs == this) {
      return *this;
    }

    pos = rhs.pos;
    len = rhs.len;
    buffer = rhs.buffer;

    return *this;
  }

  json_boolean(json_boolean&& rhs): pos{ rhs.pos }, len{ rhs.len }
  {
    buffer = std::move(rhs.buffer);
  }

  json_boolean& operator=(json_boolean&& rhs)
  {
    pos = rhs.pos;
    len = rhs.len;
    buffer = std::move(rhs.buffer);

    return *this;
  }

  bool boolean_value() const noexcept override
  {
    auto str = buffer->substr(pos, len);
    if (0 == str.compare("true")) {
      return true;
    } else {
      return false;
    }
  }

  std::string string_value() const noexcept override
  {
    return buffer->substr(pos, len);
  }

protected:
  value_type type() const noexcept override
  {
    return value_type::boolean;
  }

private:
  unsigned int pos;
  unsigned int len;
  buffer_ptr buffer;
};

MUDI_N_NS_END

// gap

MUDI_N_NS_BEGIN

void Ensures(bool pred)
{
  assert(pred);
}

class string_gap
{
public:
  typedef unsigned int gap_delta;

  void push_back(unsigned int begin, unsigned int len)
  {
    if (gaps.empty()) {
      gaps.push_back(std::make_pair(begin, len));
    } else {
      auto& last = gaps.back();
      if (last.first + last.second >= begin) {
        last.second = std::max(last.second, (begin - last.first) + len);
      } else {
        gaps.push_back(std::make_pair(begin, len));
      }
    }
  }

  bool can_collapse_at_pos(unsigned int pos) const
  {
    if (gaps.empty()) {
      return false;
    } else {
      const auto& last = gaps.back();
      return last.first + last.second > pos;
    }
  }

  gap_delta remove_gaps(buffer_ptr buffer)
  {
    if (gaps.empty()) {
      return 0;
    }

    gap_delta delta = 0;
    unsigned int origin_length = buffer->length();
    for (auto rite = gaps.rbegin(); rite != gaps.rend(); ++rite) {
      buffer->erase(rite->first, rite->second);
      delta += rite->second;
    }

    Ensures(delta == (origin_length - buffer->length()));
    gaps.clear();

    return delta;
  }

private:
  std::vector<std::pair<unsigned int, unsigned int>> gaps;
};

MUDI_N_NS_END

// parser

MUDI_N_NS_BEGIN

class json_parser
{
public:
  json_parser(const std::string& input): cursor{ 0 }
  {
    buffer = std::make_shared<std::string>(input);
    end = input.length();
  }

  json_parser(const char* input, unsigned int len): cursor{ 0 }, end{ len }
  {
    buffer = std::make_shared<std::string>(input, len);
  }

  json_parser(const json_parser&) = delete;
  json_parser& operator=(const json_parser&) = delete;

  std::unique_ptr<json_value> parse()
  {
    while(cursor < end) {
      auto type = get_next_value_type();
      switch (type) {
        case value_type::array:
        return parse_array();

        case value_type::boolean:
        return parse_boolean();

        case value_type::number:
        return parse_number();

        case value_type::object:
        return parse_object();

        case value_type::string:
        return parse_string();

        case value_type::null:
        break;
      }
    }
    return std::make_unique<json_value>();
  }

private:
  value_type get_next_value_type()
  {
    for(;;) {
      char c = peek_next_or_throw();
      c = tolower(c);

      if (isspace(c)) {
        ++cursor;
        continue;
      } else if (c == '{') {
        return value_type::object;
      } else if (c == '[') {
        return value_type::array;
      } else if (c == '\"') {
        return value_type::string;
      } else if (c == 't' || c == 'f') {
        return value_type::boolean;
      } else if (isdigit(c)) {
        return value_type::number;
      } else if (c == '-') {
        char next = peek_next_or_throw(1);
        if (isdigit(next)) {
          return value_type::number;
        } else {
          throw unknown_input();
        }
      } else {
        throw unknown_input();
      }
    }
  }

  char peek_next_or_throw(unsigned int delta = 0) const
  {
    if (cursor + delta < end) {
      return buffer->at(cursor + delta);
    }
    throw eof();
  }

  char peek_next_nonspace_or_throw(unsigned int* pos = nullptr) const
  {
    for (unsigned int i = cursor; i < end; ++i) {
      char c = buffer->at(i);
      if (!isspace(c)) {
        if (pos != nullptr) {
          *pos = i;
        }
        return c;
      }
    }
    throw eof();
  }

  char get_next_or_throw()
  {
    char ret = peek_next_or_throw();
    cursor += 1;
    return ret;
  }

  char get_next_nonspace_or_throw()
  {
    for (;;) {
      if (cursor < end) {
        char c = buffer->at(cursor);
        cursor += 1;
        if (!isspace(c)) {
          return c;
        }
      } else {
        throw eof();
      }
    }
  }

  std::unique_ptr<json_value> parse_number()
  {
    unsigned int begin = cursor;

    char _ = get_next_or_throw();   // '-' or digit
    for (;;) {
      char c;
      try {
        c = peek_next_or_throw();   // eof also treated as end of number object
      } catch (...) {
        if (buffer->at(cursor - 1) == '-') {
          throw unknown_input();
        }

        json_number* n = new json_number(begin, cursor - begin, buffer);
        return std::unique_ptr<json_value>(n);
      }

      if (!isdigit(c)) {
        c = tolower(c);
        if (c == '.' && (cursor > 0 && buffer->at(cursor - 1) != '-')) {
          ++cursor;
          continue;
        } else if (c == 'e') {
          ++cursor;
          char next = get_next_or_throw();
          if (next == '+' || next == '-' || isdigit(next)) {
            buffer->at(cursor - 2) = c;
            continue;
          } else {
            throw unknown_input();
          }
        } else {
          json_number* n = new json_number(begin, cursor - begin, buffer);
          return std::unique_ptr<json_value>(n);
        }
      }
      ++cursor;
    }
  }

  std::unique_ptr<json_value> parse_boolean()
  {
    unsigned int begin = cursor;

    char c1 = get_next_or_throw();
    c1 = tolower(c1);

    char c2 = get_next_or_throw();
    c2 = tolower(c2);

    char c3 = get_next_or_throw();
    c3 = tolower(c3);

    char c4 = get_next_or_throw();
    c4 = tolower(c4);

    if (c1 == 't' && c2 == 'r' && c3 =='u' && c4 == 'e') {
      buffer->at(begin) = c1;
      buffer->at(begin + 1) = c2;
      buffer->at(begin + 2) = c3;
      buffer->at(begin + 3) = c4;

      json_boolean* b = new json_boolean(begin, 4, buffer);
      return std::unique_ptr<json_value>(b);
    } else if (c1 == 'f' && c2 == 'a' && c3 =='l' && c4 == 's') {
      char c5 = get_next_or_throw();
      c5 = tolower(c5);
      if (c5 == 'e') {
        buffer->at(begin) = c1;
        buffer->at(begin + 1) = c2;
        buffer->at(begin + 2) = c3;
        buffer->at(begin + 3) = c4;
        buffer->at(begin + 4) = c5;

        json_boolean* b = new json_boolean(begin, 5, buffer);
        return std::unique_ptr<json_value>(b);
      }
    }
    throw unknown_input();
  }

  std::unique_ptr<json_value> parse_string();
  std::unique_ptr<json_value> parse_array();
  std::unique_ptr<json_value> parse_object();

private:
  unsigned int cursor;
  unsigned int end;
  buffer_ptr buffer;
};

std::unique_ptr<json_value> json_parser::parse_string()
{
  Ensures(buffer->at(cursor) == '\"');   // precondition

  cursor += 1;
  unsigned int begin = cursor;

  string_gap gaps;
  auto append_gap = [=, &gaps](unsigned int begin1, unsigned begin2, char escaped) {
    if (gaps.can_collapse_at_pos(begin1)) {
      this->buffer->at(begin2) = escaped;
      gaps.push_back(begin1, 1);
    } else {
      this->buffer->at(begin1) = escaped;
      gaps.push_back(begin2, 1);
    }
  };

  for (;;) {
    char c = get_next_or_throw();

    if (c == '\\') {
      char next = get_next_or_throw();
      switch (next) {
        case '\"':
        append_gap(cursor - 2, cursor - 1, '\"');
        break;

        case 'r':
        append_gap(cursor - 2, cursor - 1, '\r');
        break;

        case 'n':
        append_gap(cursor - 2, cursor - 1, '\n');
        break;

        case 't':
        append_gap(cursor - 2, cursor - 1, '\t');
        break;

        case 'b':
        append_gap(cursor - 2, cursor - 1, '\b');
        break;

        case '\\':
        append_gap(cursor - 2, cursor - 1, '\\');
        break;

        case 'f':
        append_gap(cursor - 2, cursor - 1, '\f');
        break;

        case 'u': {
          char u1 = get_next_or_throw();
          char u2 = get_next_or_throw();
          char u3 = get_next_or_throw();
          char u4 = get_next_or_throw();
          break;
        }
      }
    }

    if (c == '\"') {
      break;
    }
  }

  Ensures(buffer->at(cursor - 1) == '\"');  // postcondition

  string_gap::gap_delta delta = gaps.remove_gaps(buffer);
  cursor -= delta;
  end -= delta;

  json_string* s = new json_string(begin, cursor - begin - 1, buffer);
  return std::unique_ptr<json_value>(s);
}

std::unique_ptr<json_value> json_parser::parse_array()
{
  char h = get_next_or_throw();
  Ensures(h == '[');

  auto array = std::make_unique<json_array>();

  if (peek_next_nonspace_or_throw() == ']') {
    return std::unique_ptr<json_value>(array.release());
  }

  for (;;) {
    auto val = parse();
    array->push_back(val.release());

    char c = get_next_nonspace_or_throw();
    if (c == ',') {
      unsigned int pos = 0;
      char next = peek_next_nonspace_or_throw(&pos);
      if (next == ']') {
        cursor = pos;
        return std::unique_ptr<json_value>(array.release());
      }
    } else if (c == ']') {
      return std::unique_ptr<json_value>(array.release());
    }
  }
}

std::unique_ptr<json_value> json_parser::parse_object()
{
  char h = get_next_or_throw();
  Ensures(h == '{');

  auto obj = std::make_unique<json_object>();

  if (peek_next_nonspace_or_throw() == '}') {
    ++cursor;
    return std::unique_ptr<json_value>(obj.release());
  }

  for (;;) {
    auto key = parse();
    Ensures(key->is_string());

    char c = get_next_nonspace_or_throw();
    if (c == ':') {
      auto val = parse();
      obj->push_back(key->string_value(), val.release());
    }

    c = get_next_nonspace_or_throw();
    if (c == ',') {
      unsigned int pos = 0;
      c = peek_next_nonspace_or_throw(&pos);
      if (c == '}') {
        cursor = pos;
        return std::unique_ptr<json_value>(obj.release());
      }
    } else if (c == '}') {
      return std::unique_ptr<json_value>(obj.release());
    }
  }
}

MUDI_N_NS_END

MUDI_NS_BEGIN

std::unique_ptr<json_value> parse_string(const std::string& str)
{
  if (str.empty()) {
    return std::make_unique<json_value>();
  }
  json_parser p{ str };
  return p.parse();
}

std::unique_ptr<json_value> parse_cstring(const char* cstr, unsigned int len)
{
  if (len == 0) {
    return std::make_unique<json_value>();
  }
  json_parser p{cstr, len};
  return p.parse();
}

MUDI_NS_END

// in case macro leak

#undef MUDI_NS_BEGIN
#undef MUDI_NS_END
#undef MUDI_N_NS_BEGIN
#undef MUDI_N_NS_END
#endif
