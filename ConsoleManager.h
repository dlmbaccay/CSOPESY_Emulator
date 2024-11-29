#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <map>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include "Process.h"
#include "Scheduler.h"
#include "ConfigManager.h"

class ConsoleManager {
private:
    std::map<std::string, Process*> processes;

	ConfigManager* configManager;
    Scheduler* scheduler;
		MemoryAllocator* memoryAllocator;

	string currentSessionName;
    bool inSession = false;
    bool initialized = false;

    bool schedulerTestRun = false;

    std::mutex processMutex;

public:
    ConsoleManager();
    ~ConsoleManager();

	void printHeader();

    bool createProcess(const std::string& name);

    bool reattachProcess(const std::string& name);

    void listProcess();

    void schedulerTest();
    void schedulerTestStop() { schedulerTestRun = false; };

    void reportUtil();

		void vmStat() const;
    void processSMI();

    void initialize();
	bool handleCommand(const std::string& command);

	Scheduler* getScheduler() const { return scheduler; }
	ConfigManager* getConfig() const { return configManager; }
    std::map<std::string, Process*> getProcesses() const { return processes; }
	string getCurrentSessionName() const { return currentSessionName; }
	bool isInSession() const { return inSession; }

	void setCurrentSessionName(const std::string& name) { currentSessionName = name; }


};

#endif // ConsoleManager