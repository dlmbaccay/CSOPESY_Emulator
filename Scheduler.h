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
#include "MemoryAllocator.h"

class Scheduler {

private:
  MemoryAllocator* memAllocator;

    ConfigManager::SchedulerType schedulerType;
    int quantumCycles;  // For Round-Robin scheduling
	  int delayPerExec;  // Delay between each instruction execution
	  int batchProcessFreq;  // Frequency of batch process creation

    std::queue<Process*> readyQueue; // All processes ready to go once a thread yields
	  std::map<std::string, std::thread> runningProcesses; // Processes currently running
    std::vector<std::shared_ptr<Process>> finishedProcesses; // Add finished processes here
    std::vector<bool> cpuCores;  // Keeps track of available CPU cores
    std::mutex schedulerMutex;
    bool stopScheduler = false;

    void fcfsLoop();
    void rrLoop();

public:
    Scheduler(ConfigManager* newConfig, MemoryAllocator* resManager);
    ~Scheduler();

    struct CpuUtilization
    {
        double utilization;
        int availableCores;
        int usedCores;
    };


    void addProcess(Process* newProcess);
    void start();

    CpuUtilization getCpuUtilization();
    void displayCpuUtilization();
};

#endif // SCHEDULER_H
