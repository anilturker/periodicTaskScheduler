#ifndef PERIODIC_TASK_SCHEDULER_H
#define PERIODIC_TASK_SCHEDULER_H

#include <functional>
#include <map>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <deque>
#include <sys/time.h> // For timeval

struct TaskInfo 
{
    std::chrono::seconds interval; // Interval at which the task should run
    timeval nextExecutionTime; // Next execution time of the task
    int taskId; // Unique ID of the task
};

class PeriodicTaskScheduler {
public:
    
    // Constructor to initialize the scheduler with a specified number of threads
    PeriodicTaskScheduler(int numThreads = 5);

    // Destructor to stop the scheduler and clean up resources
    ~PeriodicTaskScheduler();

    using Task = std::function<void()>;

    // Set the function that will be periodically executed
    void setTaskFunction(Task taskFunction);

    // Add a task to the scheduler with a specified interval
    int addTask(std::chrono::seconds interval);

    // Remove a task from the scheduler using its task ID
    void removeTask(int taskId);

    // Change the interval of an existing task
    void changeTaskInterval(int taskId, std::chrono::seconds newInterval);

    // Update the scheduler with an externally provided time
    void onNewTime(const timeval &externalTime);

    // Start the scheduler in online mode
    void start();

    // Stop the scheduler
    void stop();

private:
    std::map<int, TaskInfo> tasks_; // Map of task IDs to task information
    Task taskFunction_; // The task function to be periodically executed
    std::atomic<int> nextTaskId_; // Counter for generating unique task IDs
    std::mutex mutex_; // Mutex for synchronizing access to shared resources
    std::condition_variable task_updated_; // Condition variable to signal task updates
    std::condition_variable task_available_to_execute_; // Condition variable to signal task availability
    std::deque<TaskInfo> tasksExecQueue_; // Queue of tasks ready to be executed
    std::vector<std::thread> threads_; // Vector of threads for executing tasks
    timeval currentTime_; // Current time based on the latest external update
    std::atomic<bool> running_; // Flag to indicate if the scheduler is running

    // Function that runs in each thread to execute tasks
    void threadRun();

    // Main loop for the scheduler to update tasks and manage execution
    void schedulerLoop();

    // Update the next execution time for a task
    void updateNextExecutionTime(TaskInfo &taskInfo, const timeval &currentTime);

    // Check if the current time has elapsed beyond the next execution time of a task
    bool timeElapsed(const timeval &currentTime, const timeval &nextExecutionTime);
};

#endif // PERIODIC_TASK_SCHEDULER_H
