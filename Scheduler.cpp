#include "Scheduler.h"
#include "ConfigManager.h"
#include <iostream>
#include <chrono>

Scheduler::Scheduler(ConfigManager* config) {
    
	cpuCores.resize(config->getNumCpu(), false);
	quantumCycles = config->getQuantumCycles();
	delayPerExec = config->getDelayPerExec();
	batchProcessFreq = config->getBatchProcessFreq();
	schedulerType = config->getSchedulerType();
	this->start();  // Start the scheduler loop
	
}

Scheduler::~Scheduler() {
    
}

void Scheduler::start() {
	if (schedulerType == ConfigManager::SchedulerType::FCFS) {
		std::thread schedulerThread(&Scheduler::fcfsLoop, this);
		schedulerThread.detach();
	}
	else if (schedulerType == ConfigManager::SchedulerType::RR) {
		std::thread schedulerThread(&Scheduler::rrLoop, this);
		schedulerThread.detach();
	}
	else {
		std::cerr << "Invalid scheduler type: " << schedulerType << std::endl;
		return;
	}
}

void schedulerTest() {
	int i = 1;
	while (true) {
		string processName = "process" + i;
		
	}
}

void Scheduler::addProcess(Process* newProcess) {
	readyQueue.push(newProcess);
}

Scheduler::CpuUtilization Scheduler::getCpuUtilization() {
	int availableCpuCores = std::count(cpuCores.begin(), cpuCores.end(), false);
	double utilization = (static_cast<double>(cpuCores.size()) - availableCpuCores) / static_cast<double>(cpuCores.size()) * 100;
	return { utilization, availableCpuCores, static_cast<int>(cpuCores.size()) - availableCpuCores };
}

void Scheduler::displayCpuUtilization() {
	CpuUtilization util = getCpuUtilization();

	cout << "CPU utilization: " << util.utilization << "%" << endl;
	cout << "Cores used: " << util.usedCores << endl;
	cout << "Cores available: " << util.availableCores << endl << endl;
}

void Scheduler::fcfsLoop() {
	while(true){
		std::lock_guard<std::mutex> lock(schedulerMutex);
		
		for (int i = 0; i < cpuCores.size(); i++) {
			if (!cpuCores[i] && !readyQueue.empty()) {
				Process* process = readyQueue.front();
				readyQueue.pop();
				cpuCores[i] = true;

				process->setCoreIndex(i);

				runningProcesses[process->getProcessName()] = std::thread([process, this] {
					process->setStatus(Process::RUNNING);
					process->setTimestamp();

					int cpuCycle = 0;

					while (process->getStatus() != Process::FINISHED) {
						if (delayPerExec == 0) {
							process->execute();
							process->getNextCommand();
						}
						else if ((cpuCycle + 1) % (delayPerExec + 1) == 0) {
							process->execute();
							process->getNextCommand();
						}
						else {
							// Busy-waiting cycle
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						cpuCycle++;
					}

					std::lock_guard<std::mutex> lock(schedulerMutex);
					cpuCores[process->getCoreIndex()] = false;
					process->setCoreIndex(-1);
					finishedProcesses.push_back(std::shared_ptr<Process>(process));
					});

				runningProcesses[process->getProcessName()].detach();
			}
		}

	}
}

void Scheduler::rrLoop() {
	while (true) {
		std::lock_guard<std::mutex> lock(schedulerMutex);

		for (int i = 0; i < cpuCores.size(); i++) {
			if (!cpuCores[i] && !readyQueue.empty()) {
				Process* process = readyQueue.front();
				readyQueue.pop();
				cpuCores[i] = true;

				process->setCoreIndex(i);

				runningProcesses[process->getProcessName()] = std::thread([process, this] {
					process->setStatus(Process::RUNNING);
					process->setTimestamp();
					
					int cpuCycle = 0; // number of cpu cycles since process started
					int executionCount = 0; // number of instructions executed by process, quantumCycles = number of instructions to execute

					
					while ((process->getStatus() != Process::FINISHED) && executionCount < quantumCycles) {
						if (delayPerExec == 0) { // if delay is 0, no waiting cpu cycle for process; executes instructions immediately
							process->execute();
							process->getNextCommand();
							executionCount++;
						} 
						else if ((cpuCycle + 1) % (delayPerExec + 1) == 0) { // if delay is not 0, waiting cpu cycle for process
							process->execute();
							process->getNextCommand();
							executionCount++;
						}
						else { 
							// Busy-waiting cycle
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						cpuCycle++;
						
					}

					std::lock_guard<std::mutex> lock(schedulerMutex);
					cpuCores[process->getCoreIndex()] = false;
					if (process->getStatus() == Process::FINISHED) {
						process->setCoreIndex(-1);
						finishedProcesses.push_back(std::shared_ptr<Process>(process));
					}
					else {
						process->setStatus(Process::READY);
						process->setCoreIndex(-1);
						readyQueue.push(process);
					}
					
					});

				runningProcesses[process->getProcessName()].detach();
			}
		}

	}
}