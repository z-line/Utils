add_library(utils OBJECT
    Logger.cpp
    Timer.cpp
    System.cpp
    TimerPool.cpp
    System/PathWatcher.cpp
    Network/NetInterfaceMonitor.cpp
    StringHelper.cpp)
target_include_directories(utils PUBLIC . Type Network System)
