#ifndef __LOGGER_H
#define __LOGGER_H

#define __FILENAME__ getFileName(__FILE__)

typedef enum {
  LOG_LEVEL_NONE = 0,  // no log
  LOG_LEVEL_ERROR = 1,
  LOG_LEVEL_WARNING = 2,
  LOG_LEVEL_INFO = 3,
  LOG_LEVEL_DEBUG = 4,
  LOG_LEVEL_VERBOSE = 5
} LogLevel_TypeDef;

#ifdef __cplusplus

#include "base_type.h"
#include <sstream>

#define LOG_E() Logger(LOG_LEVEL_ERROR, __FILENAME__, __LINE__)
#define LOG_W() Logger(LOG_LEVEL_WARNING, __FILENAME__, __LINE__)
#define LOG_I() Logger(LOG_LEVEL_INFO, __FILENAME__, __LINE__)
#define LOG_D() Logger(LOG_LEVEL_DEBUG, __FILENAME__, __LINE__)
#define LOG_V() Logger(LOG_LEVEL_VERBOSE, __FILENAME__, __LINE__)

class Logger {
 public:
  Logger(unsigned int level, const char* filename, const int line);
  ~Logger();

  template <typename T>
  Logger& operator<<(const T& input) {
    m_log_output << input;
    return (*this);
  }

  Logger& operator<<(u8 input) {
    m_log_output << static_cast<int>(input);
    return (*this);
  }

  Logger& operator<<(bool input);

 private:
  std::stringstream m_log_output;
  unsigned int m_level;
  const char* m_filename;
  const int m_line;
};

#else
#define LOG_E(b, ...) \
  my_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, b, ##__VA_ARGS__)
#define LOG_W(b, ...) \
  my_log(LOG_LEVEL_WARNING, __FILE__, __LINE__, b, ##__VA_ARGS__)
#define LOG_I(b, ...) \
  my_log(LOG_LEVEL_INFO, __FILE__, __LINE__, b, ##__VA_ARGS__)
#define LOG_D(b, ...) \
  my_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, b, ##__VA_ARGS__)
#define LOG_V(b, ...) \
  my_log(LOG_LEVEL_VERBOSE, __FILE__, __LINE__, b, ##__VA_ARGS__)

void my_log(LogLevel_TypeDef log_level, const char* const tag, const int line,
            const char* format, ...);
#endif

#ifdef __cplusplus
extern "C" {
#endif
const char* getFileName(const char* path);
#ifdef __cplusplus
}
#endif

#endif
