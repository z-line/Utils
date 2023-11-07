add_library(utils OBJECT Logger.cpp Timer.cpp System.cpp TimerPool.cpp)
target_include_directories(utils INTERFACE .)
