add_library(utils OBJECT Logger.cpp Timer.cpp System.cpp TimerPool.cpp)
target_include_directories(utils INTERFACE .)
add_subdirectory(DataStruct)
add_subdirectory(Logger)
