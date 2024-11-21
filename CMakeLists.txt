add_library(utils INTERFACE)
target_sources(utils INTERFACE
    Logger.cpp
    Timer.cpp
    System.cpp
    TimerPool.cpp
    System/PathWatcher.cpp
    Network/NetInterfaceMonitor.cpp
    StringHelper.cpp)
target_include_directories(utils INTERFACE . Type Network System)
