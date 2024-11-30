#include "Scheduler.h"
#include "ConfigManager.h"
#include <iostream>
#include <chrono>

Scheduler::Scheduler(ConfigManager* config, MemoryAllocator* resManager) {
	memAllocator = resManager;
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
	std::chrono::time_point<std::chrono::high_resolution_clock> lastIdleTickTime = std::chrono::high_resolution_clock::now();

	while (true) {
		std::lock_guard<std::mutex> lock(schedulerMutex);
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastIdleTickTime);

		for (int i = 0; i < cpuCores.size(); i++) {
			if (!cpuCores[i] && !readyQueue.empty()) {
				//runningProcesses.find(process->getProcessName()) == runningProcesses.end() &&
				Process* process = readyQueue.front();
				// Attempt to allocate memory if the process isn't already in memory
				if (!memAllocator->isProcessInMemory(process)) {
					if (memAllocator->backingStoreSet.contains(process->getProcessName())) {
						process = memAllocator->loadProcessFromBackingStore(process->getProcessName());
					}
					if (!memAllocator->allocateMemory(process)) {
						// Memory allocation failed, so re-queue to try later
						readyQueue.push(readyQueue.front());
						readyQueue.pop();
						continue;
					}
				}
				// Memory allocation successful or process already in memory
				readyQueue.pop();
				cpuCores[i] = true;
				process->setCoreIndex(i);

				// Start a thread for the process execution
				runningProcesses[process->getProcessName()] = std::thread([process, this] {
					process->setStatus(Process::RUNNING);
					process->setTimestamp();

					int cpuCycle = 0;
					int executionCount = 0;

					// Execute process instructions within its quantum
					while ((process->getStatus() != Process::FINISHED) && executionCount < quantumCycles) {
						if (delayPerExec == 0) {
							process->execute();
							process->getNextCommand();
							executionCount++;
							activeCpuTicks += 1;
						}
						else if ((cpuCycle + 1) % (delayPerExec + 1) == 0) {
							process->execute();
							process->getNextCommand();
							activeCpuTicks += 1;
							executionCount++;
						}
						
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
						cpuCycle++;
					}

					std::lock_guard<std::mutex> lock(schedulerMutex);

					cpuCores[process->getCoreIndex()] = false;

					if (process->getStatus() == Process::FINISHED) {
						// Process completed, remove from memory and move to finished queue
						memAllocator->deallocateMemory(process);
						finishedProcesses.push_back(std::shared_ptr<Process>(process));
						runningProcesses.erase(process->getProcessName());
					}
					else {
						// Process quantum expired, re-queue without deallocating memory
						process->setStatus(Process::READY);
						readyQueue.push(process);
					}

					});

				// Detach the thread to allow it to run independently
				runningProcesses[process->getProcessName()].detach();
			}
			else if (!cpuCores[i]) {
				if (duration.count() >= 20) {
					idleCpuTicks += 1;
					lastIdleTickTime = std::chrono::high_resolution_clock::now();
				}
			}
		}
	}
}
