#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>

using namespace std;

class ConfigManager
{
public:
	enum SchedulerType { FCFS, RR };  // Scheduler types: First-Come-First-Serve, Round-Robin

	ConfigManager();
	~ConfigManager();

	void readConfig();
	void displayConfig();

	int getNumCpu() { return numCpu; };
	SchedulerType getSchedulerType() { return schedulerType; };
	int getQuantumCycles() { return quantumCycles; };
	int getBatchProcessFreq() { return batchProcessFreq; };
	int getMinIns() { return minIns; };
	int getMaxIns() { return maxIns; };
	int getDelayPerExec() { return delayPerExec; };
	int getMaxOverallMem() { return maxOverallMem; };
	int getMemPerFrame() { return memPerFrame; };
	int getMinMemPerProcess() { return minMemPerProcess; };
	int getMaxMemPerProcess() { return maxMemPerProcess; };

private:
	int numCpu;
	SchedulerType schedulerType;
	int quantumCycles;
	int batchProcessFreq;
	int minIns;
	int maxIns;
	int delayPerExec;
	int maxOverallMem;
	int memPerFrame;
	int minMemPerProcess;
	int maxMemPerProcess;
};
#endif // CONFIGMANAGER_H
