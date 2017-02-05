#ifndef MUDI_JSON_HPP
#define MUDI_JSON_HPP

#include <unordered_map>
#include <vector>
#include <string>
#include <utility>  // make_pair

#include <memory>
#include <exception>

#define MUDI_NS_BEGIN namespace mudi {
#define MUDI_NS_END   }

#define MUDI_N_NS_BEGIN namespace mudi { namespace detail {
#define MUDI_N_NS_END   }}

// base value

MUDI_N_NS_BEGIN

class json_base_value: public json_value
{
public:
  json_base_value(unsigned int pos, unsigned int len, buffer_ptr buf): pos{ pos }, len{ len }, buffer{ buf }
  {}

  json_base_value(const json_string& rhs): pos{ rhs.pos }, len{ rhs.len }, buffer{ rhs.buffer }
  {}

  json_base_value& operator=(const json_string& rhs)
  {
    if (&rhs == this) {
      return *this;
    }

    auto tmp = rhs;
    std::swap(*this, tmp);
    return *this;
  }

  json_base_value(json_string&& rhs): pos{ rhs.pos }, len{ rhs.len }
  {
    buffer = std::move(rhs.buffer);
  }

  json_base_value& operator=(json_string&& rhs)
  {
    pos = rhs.pos;
    len = rhs.len;
    buffer = std::move(rhs.buffer);
  }

protected:
  unsigned int pos;
  unsigned int len;
  buffer_ptr buffer;
}

MUDI_N_NS_END

// string

MUDI_N_NS_BEGIN

class json_value;

typedef std::shared_ptr<std::string> buffer_ptr;

enum class value_type
{
  array,
  object,
  string,
  number,
  boolean,
  null,
};

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
  }

  std::string string_value() noexcept const override
  {
      return buffer.substr(pos, len);
  }

protected:
  value_type type() noexcept const override
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

  json_value& operator[](unsigned int index) const override
  {
    return data.at(index);
  }

protected:
  value_type type() noexcept const override
  {
    return value_type::array;
  }

private:
  void append(json_value val) noexcept
  {
    data.push_back(val);
  }

private:
  std::vector<json_value> data;
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

  json_value& operator[](const std::string& key) noexcept const override
  {
    auto found = data.find(key);
    if (found == data.end()) {
      return json_null{};
    } else {
      return *found;
    }
  }

  bool append(const std::string& key, const json_value& value) noexcept
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
  value_type type() noexcept const override
  {
    return value_type::object;
  }

private:
  std::unordered_map<std::string, json_value> data;
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
      reutrn *this;
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

  int int_value() noexcept const override
  {
    return 0;
  }

  double double_value() noexcept const override
  {
    return 0;
  }

protected:
  value_type type() noexcept const override
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

};

MUDI_N_NS_END

// null

MUDI_N_NS_BEGIN

class json_null: public json_value
{
// public:
//   json_null(const json_null&) = delete;
//   json_null& operator=(const json_null&) = delete;

protected:
  value_type type() noexcept const override
  {
    return value_type::null;
  }
};

MUDI_N_NS_END

// interface

MUDI_NS_BEGIN

class json_value
{
public:
  virtual ~json_value() noexcept;

  virtual json_value& operator[](const std::string& key) noexcept const
  {
    return json_null{};
  }

  virtual json_value& operator[](unsigned int index) const
  {
    return json_null{};
  }

  virtual bool boolean_value() noexcept const
  {
    return false;
  }

  virtual std::string string_value() noexcept const
  {
    return "null";
  }

  virtual int int_value() noexcept const
  {
    return 0;
  }

  virtual double double_value() noexcept const
  {
    return 0;
  }

  bool is_bool() noexcept const
  {
    return type() == value_type::boolean;
  }

  bool is_array() noexcept const
  {
    return type() == value_type::array;
  }

  bool is_string() noexcept const
  {
    return type() == value_type::string;
  }

  bool is_number() noexcept const
  {
    return type() == value_type::number;
  }

  bool is_object() noexcept const
  {
    return type() == value_type::object;
  }

  bool is_null() noexcept const
  {
    return type() == value_type::null;
  }

protected:
  virtual value_type type() = 0;
};

std::unique_ptr<json_value> parse_string(const std::string& str);
std::unique_ptr<json_value> parse_cstring(const char* cstr, unsigned int len);

MUDI_NS_END

#endif
