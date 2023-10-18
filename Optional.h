#ifndef __OPTIONAL_H
#define __OPTIONAL_H

#ifdef __cplusplus

template <typename T>
class Optional {
 public:
  Optional() : m_value(), m_is_set(false) {}

  Optional(const Optional& copy)
      : m_value(copy.m_value), m_is_set(copy.m_is_set) {}

  Optional(const T& value) : m_value(value), m_is_set(true) {}

  Optional(T&& value) : m_value(value), m_is_set(true) {}

  Optional& operator=(const Optional& copy) {
    m_value = copy.m_value;
    m_is_set = copy.m_is_set;
    return (*this);
  }

  Optional& operator=(const T& value) {
    m_value = value;
    m_is_set = true;
    return (*this);
  }

  bool operator==(const T& value) const {
    return (m_is_set && (m_value == value));
  }

  bool operator==(const Optional& value) const {
    return ((m_is_set == value.m_is_set) && (m_value == value.m_value));
  }

  bool operator<(const T& value) const {
    return (m_is_set && (m_value < value));
  }

  bool operator<(const Optional& value) const {
    return ((m_is_set == value.m_is_set) && (m_value < value.m_value));
  }

  bool operator>(const T& value) const {
    return (m_is_set && (m_value > value));
  }

  bool operator>(const Optional& value) const {
    return ((m_is_set == value.m_is_set) && (m_value > value.m_value));
  }

  operator const T&() const { return m_value; }

  operator T&() {
    m_is_set = true;
    return m_value;
  }

  const T& value() const { return m_value; }

  T& value() {
    m_is_set = true;
    return m_value;
  }

  void setValue(T& value) {
    m_is_set = true;
    m_value = value;
  }

  bool isSet() const { return m_is_set; }

  void clear() { m_is_set = false; }

 private:
  T m_value;
  bool m_is_set;
};

#else

#endif

#endif
