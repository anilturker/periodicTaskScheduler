# Add include directories for the source files
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add the scheduler source files
add_library(PeriodicTaskScheduler STATIC PeriodicTaskScheduler.cpp)

# Ensure the main executable is created and linked with the PeriodicTaskScheduler library
add_executable(main main.cpp)
target_link_libraries(main PeriodicTaskScheduler)
