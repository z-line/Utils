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

  void setValue(T value) {
    m_isSet = true;
    m_value = value;
  }

  const T& getValue() { return m_value; }

 private:
  bool m_isSet = false;
  T m_value;
};

#else

#endif

#endif
