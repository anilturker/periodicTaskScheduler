# Periodic Task Scheduler

This project implements a periodic task scheduler in C++. The scheduler allows you to add, remove, and update tasks that run at specified intervals. It supports starting and stopping the scheduler, updating the time externally, and ensuring tasks are executed at the correct intervals.

## Getting Started

### Prerequisites

- C++17 or later
- CMake 3.20 or later
- Boost.Test library(==1.85.0)

### Building the Project

1. **Clone the repository:**
   ```sh
   git clone https://github.com/anilturker/periodicTaskScheduler.git
   cd periodicTaskScheduler
   ```
2. **Create a build directory and navigate into it:**
   ```
   mkdir build
   cd build
   ```
3. **Run CMake to configure the project:**
   ```
   cmake ..
   ```
4. **Build the project using Make:**
   ```
   make
   ```
### Running the Tests
1. **Navigate to the build directory:**
   ```sh
   cd build
   ```
2. **Run the tests using the test executable:**
   ```
   ./tests/PeriodicTaskSchedulerTest --log_level=all
   ```
### License

This project is licensed under the MIT License - see the LICENSE file for details.
### Acknowledgements

Boost.Test for the testing framework
CMake for build configuration



