#ifndef MUDI_JSON_HPP
#define MUDI_JSON_HPP

#include <unordered_map>
#include <vector>
#include <string>

#include <utility>  // make_pair
#include <memory>   // share_ptr
#include <cctype>   // isspace
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

class premature_eof: public std::exception
{
public:
  const char* what() const noexcept override
  {
    return "Input end prematurely";
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
  virtual ~json_value() noexcept;

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

std::unique_ptr<json_value> parse_string(const std::string& str)
{
  json_parser p{ str };
  return p.parse();
}

std::unique_ptr<json_value> parse_cstring(const char* cstr, unsigned int len)
{
  json_parser p{cstr, len}
  return p.parse();
}

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

class json_array: public json_value
{
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

protected:
  value_type type() const noexcept override
  {
    return value_type::array;
  }

private:
  void append(json_value* val) noexcept
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

  bool append(const std::string& key, json_value* value) noexcept
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

  int int_value() const noexcept override
  {
    return 0;
  }

  double double_value() const noexcept override
  {
    return 0;
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

// parser

MUDI_N_NS_BEGIN

void Ensures(bool pred)
{
  assert(pred);
}

class json_parser
{
public:
  json_parser(const std::string& input): cursor{ 0 }
  {
    buffer = make_shared(input);
    end = input.length();
  }

  json_parser(const char* input, unsigned int len): cursor{ 0 }, end{ len }
  {
    buffer = make_shared(input, len);
  }

  json_parser(const json_parser&) = delete;
  json_parser& operator=(const json_parser&) = delete;

  std::unique_ptr<json_value> parse()
  {

  }

private:
  value_type get_next_value_type()
  {
    for(;;) {
      char c = peek_or_throw();
      if (isspace(c) || c == ':' || c == ',') {
        continue;
      } else if (c == '{') {
        return value_type::object;
      } else if (c == '[') {
        return value_type::array;
      } else if (c == '\"') {
        return value_type::string;
      } else if (isdigit(c)) {
        return value_type::number;
      } else if (c == '-') {
        char next = peek_or_throw(2);
        if (isdigit(next)) {
          return value_type::number;
        } else {
          throw unknown_input();
        }
      }
    }
  }

  char peek_or_throw(unsigned int delta = 1)
  {
    if (cursor + delta < end) {
      return buffer->at(cursor);
    }
    throw premature_eof();
  }

  char get_next_or_throw()
  {
    char ret = peek_or_throw();
    cursor += 1;
    return ret;
  }

  json_number parse_number();
  json_string parse_string();

private:
  unsigned int cursor;
  unsigned int end;
  buffer_ptr buffer;
};

json_number json_parser::parse_number()
{

}

json_string json_parser::pase_string()
{
  Ensures(buffer->at(cursor) == "\"");   // precondition

  cursor += 1;
  unsigned int begin = cursor;

  for (unsigned int i = cursor; i < end; ++i) {
    char c = buffer->at(i);
    if (c == "\\") {
      char next = get_next_or_throw();
      if (next == "\"") {
        continue;
      }
    }

    if (c == "\"") {
      break;
    }
  }

  Ensures(buffer->at(cursor - 1) == "\"");  // postcondition
}

MUDI_N_NS_END

#endif
