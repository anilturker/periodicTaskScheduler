find_package(Boost REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(TestPeriodicTaskScheduler TestPeriodicTaskScheduler.cpp)
target_link_libraries(TestPeriodicTaskScheduler PeriodicTaskScheduler ${Boost_LIBRARIES})

add_test(NAME TestPeriodicTaskScheduler COMMAND TestPeriodicTaskScheduler)
