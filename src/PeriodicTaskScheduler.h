#ifndef PERIODICTASKSCHEDULER_H
#define PERIODICTASKSCHEDULER_H

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <sys/time.h>

class PeriodicTaskScheduler {
public:
    using Task = std::function<void()>;

    struct PeriodicTask {
        Task task;
        timeval interval;
        timeval nextExecutionTime;
    };

    PeriodicTaskScheduler();
    ~PeriodicTaskScheduler();

    int addTask(Task task, const timeval& interval);
    void removeTask(int taskId);
    void changeTaskInterval(int taskId, const timeval& newInterval);
    void start();
    void stop();

    void onNewTime(const timeval& newTime);

private:
    void processPackets();
    timeval addTimevals(const timeval& t1, const timeval& t2);
    bool timevalGreaterOrEqual(const timeval& t1, const timeval& t2);

    std::map<int, PeriodicTask> tasks;
    std::thread processing_thread;
    std::atomic<bool> running{false};
    std::mutex tasks_mutex;
    int nextTaskId{0};
    timeval currentTime; // Class member to track the current time
};

#endif // PERIODICTASKSCHEDULER_H
