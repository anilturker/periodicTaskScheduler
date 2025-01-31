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
#include <queue>
#include <sys/time.h> // For timeval

struct TaskInfo 
{
    std::chrono::seconds interval; // Interval at which the task should run
    timeval nextExecutionTime; // Next execution time of the task
    int taskId; // Unique ID of the task

    // Compare tasks based on their next execution time for the priority queue
    bool operator>(const TaskInfo& other) const {
        if (nextExecutionTime.tv_sec == other.nextExecutionTime.tv_sec) {
            return nextExecutionTime.tv_usec > other.nextExecutionTime.tv_usec;
        }
        return nextExecutionTime.tv_sec > other.nextExecutionTime.tv_sec;
    }
};

class PeriodicTaskScheduler {
public:
    PeriodicTaskScheduler(int numThreads = 5);
    ~PeriodicTaskScheduler();

    using Task = std::function<void()>;

    void setTaskFunction(Task taskFunction);
    int addTask(std::chrono::seconds interval);
    void removeTask(int taskId);
    void changeTaskInterval(int taskId, std::chrono::seconds newInterval);
    void onNewTime(const timeval &externalTime);
    void start();
    void stop();

    // Methods for testing
    bool isTaskPresent(int taskId);
    std::chrono::seconds getTaskInterval(int taskId);
    bool isRunning();

private:
    std::map<int, TaskInfo> tasks_; // Map of task IDs to task information
    Task taskFunction_; // The task function to be periodically executed
    std::atomic<int> nextTaskId_; // Counter for generating unique task IDs
    std::mutex mutex_; // Mutex for synchronizing access to shared resources
    std::condition_variable task_updated_; // Condition variable to signal task updates and availability
    std::priority_queue<TaskInfo, std::vector<TaskInfo>, std::greater<>> taskQueue_; // Priority queue of tasks ready to be executed
    std::deque<TaskInfo> readyTasks_; // Queue of tasks that are ready to be executed
    std::vector<std::thread> threads_; // Vector of threads for executing tasks
    timeval currentTime_; // Current time based on the latest external update
    std::atomic<bool> running_; // Flag to indicate if the scheduler is running
    std::thread schedulerThread_; // Thread for the scheduler loop

    void threadRun();
    void schedulerLoop();
    void updateNextExecutionTime(TaskInfo &taskInfo, const timeval &currentTime);
    bool timeElapsed(const timeval &currentTime, const timeval &nextExecutionTime);
};

#endif // PERIODIC_TASK_SCHEDULER_H
