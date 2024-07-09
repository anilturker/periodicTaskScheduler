#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <sys/time.h> // For timeval
#include "PeriodicTaskScheduler.h"

// The task function to be executed periodically
void periodicTask() {
    std::cout << "Periodic Task executed" << std::endl;
}

int main() {
    PeriodicTaskScheduler scheduler;

    // Set the task function
    scheduler.setTaskFunction(periodicTask);

    // Add tasks with different intervals
    auto taskId1 = scheduler.addTask(std::chrono::seconds(5));
    auto taskId2 = scheduler.addTask(std::chrono::seconds(10));
    auto taskId3 = scheduler.addTask(std::chrono::seconds(15));

    // Start the scheduler
    scheduler.start();

    // Simulate processing packets with external time updates
    timeval startTime;
    gettimeofday(&startTime, nullptr);
    for (int i = 0; i < 30; ++i) {
        timeval packetTime = startTime;
        packetTime.tv_sec += i;

        // Simulate processing packets by updating the scheduler with external time
        scheduler.onNewTime(packetTime);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some delay
    }

    // Stop the scheduler
    scheduler.stop();

    return 0;
}
