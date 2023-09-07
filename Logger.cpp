#include "Logger.h"

#ifdef __cplusplus
#include <iostream>
#include <sstream>
#include <map>
#include <ctime>
#include <iomanip>
#include <string>
#include <mutex>
#include <cstring>
#include <string>
#else
#include "stdio.h"
#include "stdarg.h"
#include "time.h"
#include "string.h"
#endif

#define LOG_CTRL_RESET "\x1B[0m"  // Reset to default colors
#define LOG_CTRL_CLEAR "\x1B[2J"  // Clear screen, reposition cursor to top left

#define LOG_CTRL_TEXT_BLACK "\x1B[2;30m"
#define LOG_CTRL_TEXT_RED "\x1B[2;31m"
#define LOG_CTRL_TEXT_GREEN "\x1B[2;32m"
#define LOG_CTRL_TEXT_YELLOW "\x1B[2;33m"
#define LOG_CTRL_TEXT_BLUE "\x1B[2;34m"
#define LOG_CTRL_TEXT_MAGENTA "\x1B[2;35m"
#define LOG_CTRL_TEXT_CYAN "\x1B[2;36m"
#define LOG_CTRL_TEXT_WHITE "\x1B[2;37m"

#define LOG_CTRL_TEXT_BRIGHT_BLACK "\x1B[1;30m"
#define LOG_CTRL_TEXT_BRIGHT_RED "\x1B[1;31m"
#define LOG_CTRL_TEXT_BRIGHT_GREEN "\x1B[1;32m"
#define LOG_CTRL_TEXT_BRIGHT_YELLOW "\x1B[1;33m"
#define LOG_CTRL_TEXT_BRIGHT_BLUE "\x1B[1;34m"
#define LOG_CTRL_TEXT_BRIGHT_MAGENTA "\x1B[1;35m"
#define LOG_CTRL_TEXT_BRIGHT_CYAN "\x1B[1;36m"
#define LOG_CTRL_TEXT_BRIGHT_WHITE "\x1B[1;37m"

#define LOG_CTRL_BG_BLACK "\x1B[24;40m"
#define LOG_CTRL_BG_RED "\x1B[24;41m"
#define LOG_CTRL_BG_GREEN "\x1B[24;42m"
#define LOG_CTRL_BG_YELLOW "\x1B[24;43m"
#define LOG_CTRL_BG_BLUE "\x1B[24;44m"
#define LOG_CTRL_BG_MAGENTA "\x1B[24;45m"
#define LOG_CTRL_BG_CYAN "\x1B[24;46m"
#define LOG_CTRL_BG_WHITE "\x1B[24;47m"

#define LOG_CTRL_BG_BRIGHT_BLACK "\x1B[4;40m"
#define LOG_CTRL_BG_BRIGHT_RED "\x1B[4;41m"
#define LOG_CTRL_BG_BRIGHT_GREEN "\x1B[4;42m"
#define LOG_CTRL_BG_BRIGHT_YELLOW "\x1B[4;43m"
#define LOG_CTRL_BG_BRIGHT_BLUE "\x1B[4;44m"
#define LOG_CTRL_BG_BRIGHT_MAGENTA "\x1B[4;45m"
#define LOG_CTRL_BG_BRIGHT_CYAN "\x1B[4;46m"
#define LOG_CTRL_BG_BRIGHT_WHITE "\x1B[4;47m"

static volatile LogLevel_TypeDef m_log_level = LOG_LEVEL_INFO;
static volatile bool enable_colorful = true;

#ifdef __cplusplus
#define STRING_TYPE std::string
#else
#define STRING_TYPE char*
#endif
static const STRING_TYPE colorful_map[] = {
    [LOG_LEVEL_NONE] = "",
    [LOG_LEVEL_ERROR] = LOG_CTRL_TEXT_RED,
    [LOG_LEVEL_WARNING] = LOG_CTRL_TEXT_YELLOW,
    [LOG_LEVEL_INFO] = LOG_CTRL_TEXT_GREEN,
    [LOG_LEVEL_DEBUG] = LOG_CTRL_TEXT_BLUE,
    [LOG_LEVEL_VERBOSE] = LOG_CTRL_TEXT_CYAN,
};

void get_datetime(char* buffer, size_t buffer_len) {
  time_t timestamp = time(NULL);
  strftime(buffer, buffer_len, "%Y-%m-%d %H:%M:%S", localtime(&timestamp));
}

const char* getFileName(const char* path) {
  const char* lastSeparator = strrchr(path, '/');
  const char* fileName = (lastSeparator != NULL) ? lastSeparator + 1 : path;
  return fileName;
}

void log_hex(u8* data, size_t len) {
  for (int i = 0; i < len; i++) {
    printf("%02x ", data[i]);
    if (i >= len - 1) {
      printf("\n");
    }
  }
}

#ifdef __cplusplus

Logger::Logger(LogLevel_TypeDef level, const char* filename, const int line)
    : m_log_output(), m_level(level), m_filename(filename), m_line(line) {}

Logger::~Logger() {
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);
  if (m_level > m_log_level) {
    return;
  }
  char time_str[64];
  get_datetime(time_str, sizeof(time_str));
  std::cout << colorful_map[m_level] << "[" << time_str << "]"
            << LOG_CTRL_RESET;
  std::cout << " - " << m_filename << ":" << m_line << " - "
            << m_log_output.str() << std::endl;
}

// template <typename T>
// Logger& Logger::operator<<(const T& input) {
//       m_log_output << input;
//       return (*this);
//   }

Logger& Logger::operator<<(bool input) {
  static const char* BOOL_STRING[] = {"false", "true"};
  m_log_output << BOOL_STRING[static_cast<int>(input)];
  return (*this);
}

#else

void my_log(LogLevel_TypeDef log_level, const char* const tag, const int line,
            const char* format, ...) {
  if (log_level <= m_log_level) {
    va_list args;
    va_start(args, format);
    char datetime_str[32];
    get_datetime(datetime_str, sizeof(datetime_str));
    if (enable_colorful) {
      printf("%s[%s] - %s:%d - ", colorful_map[log_level], datetime_str, tag,
             line);
      vprintf(format, args);
      printf("%s\n", LOG_CTRL_RESET);
    } else {
      printf("[%s] - %s:%d - ", datetime_str, tag, line);
      vprintf(format, args);
      printf("\n");
    }
    fflush(stdout);
    va_end(args);
  }
}

#endif
