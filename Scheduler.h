#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Process.h"
#include "ConfigManager.h"
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <condition_variable>
#include "ConfigManager.h"

class Scheduler {

private:
    ConfigManager::SchedulerType schedulerType;
    int timeQuantum;  // For Round-Robin scheduling
	int delayPerExec;  // Delay between each instruction execution
	int batchProcessFreq;  // Frequency of batch process creation

    std::queue<Process*> readyQueue; // All processes ready to go once a thread yields
	std::map<std::string, std::thread> runningProcesses; // Processes currently running
    std::vector<std::shared_ptr<Process>> finishedProcesses; // Add finished processes here
    std::vector<bool> cpuCores;  // Keeps track of available CPU cores
    std::mutex schedulerMutex;
    std::condition_variable cv;
    bool stopScheduler = false;

    void fcfsLoop();
    void rrLoop();

public:
    Scheduler(ConfigManager* newConfig);
    ~Scheduler();

    void addProcess(Process* newProcess);
    void start();
    void stop();

    void displayCpuUtilization();


};

#endif // SCHEDULER_H
