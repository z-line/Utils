#ifndef __OPTIONAL_H
#define __OPTIONAL_H

#ifdef __cplusplus

template <typename T>
class Optional {
 public:
  Optional(T value) {
    m_isSet = true;
    m_value = value;
  }
  Optional(){};

  bool isSet() { return m_isSet; }

  void setValue(T value) {
    m_isSet = true;
    m_value = value;
  }

  const T& value() { return m_value; }

  bool operator==(const Optional& other) const {
    return m_isSet == other.m_isSet && m_value == other.m_value;
  }

  Optional& operator=(const T& value) {
    m_isSet = true;
    m_value = value;
    return *this;
  }

 private:
  bool m_isSet = false;
  T m_value;
};

#else

#endif

#endif
