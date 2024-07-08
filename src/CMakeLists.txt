# src/CMakeLists.txt
add_library(PeriodicTaskScheduler STATIC PeriodicTaskScheduler.cpp)

# Ensure the main executable is created and linked with the PeriodicTaskScheduler library
add_executable(main main.cpp)
target_link_libraries(main PeriodicTaskScheduler)
