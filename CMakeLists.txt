cmake_minimum_required(VERSION 3.20)
project(PeriodicTaskScheduler)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the Boost root directory
set(BOOST_ROOT /home/anil/boost_1_85_0)
set(BOOST_INCLUDEDIR ${BOOST_ROOT})
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)

# Set CMAKE_PREFIX_PATH to include Boost
set(CMAKE_PREFIX_PATH "${BOOST_ROOT}/stage")

# Set variables for static or dynamic linking if needed
set(Boost_USE_STATIC_LIBS OFF) # or ON if you prefer static linking
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Find Boost
find_package(Boost 1.85 REQUIRED COMPONENTS filesystem system unit_test_framework)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARYDIR})
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
else()
    message(FATAL_ERROR "Could not find Boost!")
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
