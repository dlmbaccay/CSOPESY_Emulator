#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <map>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include "Process.h"

class ScreenManager {
private:
    std::map<std::string, Process*> processes;
    std::queue<Process*> waitingQueue;  // Queue for processes waiting to be scheduled
    int coreCount;  // Number of CPU cores
    int availableCores;  // Currently available cores
    std::vector<bool> cpuCores;
    std::mutex processMutex;
    std::map<std::string, std::thread> runningProcesses;
    void scheduleProcesses();

public:
    ScreenManager(int cores);
    ~ScreenManager();

    void createProcess(const std::string& name);

    bool reattachProcess(const std::string& name);

    void listProcess();
    void startScheduler();

};

#endif // SCREENMANAGER_H