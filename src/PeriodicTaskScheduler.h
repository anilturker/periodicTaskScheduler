#ifndef PERIODICTASKSCHEDULER_H
#define PERIODICTASKSCHEDULER_H

#include <functional>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <map>

class PeriodicTaskScheduler {
public:
    using Task = std::function<void()>;

    struct PeriodicTask {
        Task task;
        std::chrono::seconds interval;
        std::chrono::time_point<std::chrono::system_clock> nextExecutionTime;
    };

    PeriodicTaskScheduler();
    ~PeriodicTaskScheduler();

    int addTask(Task task, std::chrono::seconds interval);
    void removeTask(int taskId);
    void changeTaskInterval(int taskId, std::chrono::seconds newInterval);
    void start();
    void stop();

private:
    void onNewTime(int newTime);
    void processPackets();

    std::map<int, PeriodicTask> tasks;
    std::thread processing_thread;
    std::atomic<bool> running{false};
    std::mutex tasks_mutex;
    int nextTaskId{0};
};

#endif // PERIODICTASKSCHEDULER_H
