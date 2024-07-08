# Set the output directory for the test executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# Create the test executable
add_executable(PeriodicTaskSchedulerTest PeriodicTaskSchedulerTest.cpp)

# Include Boost headers
target_include_directories(PeriodicTaskSchedulerTest PRIVATE ${Boost_INCLUDE_DIRS})

# Link the test executable with Boost and the PeriodicTaskScheduler library
target_link_libraries(PeriodicTaskSchedulerTest PRIVATE ${Boost_LIBRARIES} PeriodicTaskScheduler)

# Register the test with CTest
add_test(NAME PeriodicTaskSchedulerTest COMMAND PeriodicTaskSchedulerTest)

# Output a message indicating that the test has been added
message(STATUS "Added test PeriodicTaskSchedulerTest")
