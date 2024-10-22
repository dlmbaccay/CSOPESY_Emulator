#include "Scheduler.h"
#include "ConfigManager.h"
#include <iostream>
#include <chrono>

Scheduler::Scheduler(ConfigManager* config) {
    
	cpuCores.resize(config->getNumCpu(), false);
	timeQuantum = config->getQuantumCycles();
	delayPerExec = config->getDelayPerExec();
	batchProcessFreq = config->getBatchProcessFreq();
	schedulerType = config->getSchedulerType();
	this->start();  // Start the scheduler loop
	stopScheduler = false;
}

Scheduler::~Scheduler() {
    stop();
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
	
   /* std::thread schedulerThread(&Scheduler::schedulerLoop, this);
    schedulerThread.detach();*/
}

void schedulerTest() {
	int i = 1;
	while (true) {
		string processName = "process" + i;
		
	}
}

void Scheduler::addProcess(Process* newProcess) {
	readyQueue.push(newProcess);
    //std::cout << "Process '" << newProcess->getProcessName() << "' added to queue." << std::endl;
	cv.notify_one();  // Notify the scheduler to start
}

void Scheduler::stop() {
    {
        std::lock_guard<std::mutex> lock(schedulerMutex);
        stopScheduler = true;
    }
    cv.notify_all();  // Wake up any waiting threads to exit
}

void Scheduler::displayCpuUtilization() {
	int availableCpuCores = 0;
	for (auto core : cpuCores) {
		if (!core) {
			availableCpuCores++;
		}
	}

	float utilization = (static_cast<float>(cpuCores.size()) - availableCpuCores) / static_cast<float>(cpuCores.size());
	
	cout << "CPU utilization: " << utilization*100.0 << "%" << endl;
	cout << "Cores used: " << (cpuCores.size() - availableCpuCores) << endl;
	cout << "Cores available: " << availableCpuCores << endl << endl;
}

void Scheduler::fcfsLoop() {
	while(true){
		std::unique_lock<std::mutex> lock(schedulerMutex);
		cv.wait(lock, [this] { return !readyQueue.empty() || stopScheduler; });
		
		for (int i = 0; i < cpuCores.size(); i++) {
			if (!cpuCores[i] && !readyQueue.empty()) {
				Process* process = readyQueue.front();
				readyQueue.pop();
				cpuCores[i] = true;

				process->setCoreIndex(i);
				cout << process->getProcessName() << " assigned to core " << i << endl;
				runningProcesses[process->getProcessName()] = std::thread([process, this] {
					process->setStatus(Process::RUNNING);
					process->setTimestamp();
					while (process->getStatus() != Process::FINISHED) {
						process->execute();
						process->getNextCommand();
						std::this_thread::sleep_for(std::chrono::milliseconds(200));
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
		std::unique_lock<std::mutex> lock(schedulerMutex);
		cv.wait(lock, [this] { return !readyQueue.empty() || stopScheduler; });

		for (int i = 0; i < cpuCores.size(); i++) {
			if (!cpuCores[i] && !readyQueue.empty()) {
				Process* process = readyQueue.front();
				readyQueue.pop();
				cpuCores[i] = true;

				process->setCoreIndex(i);

				runningProcesses[process->getProcessName()] = std::thread([process, this] {
					process->setStatus(Process::RUNNING);
					process->setTimestamp();
					
					int i = 0;
					
					while ((process->getStatus() != Process::FINISHED) && i < timeQuantum) {
						process->execute();
						process->getNextCommand();
						std::this_thread::sleep_for(std::chrono::milliseconds(200));
						i++;
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
						cv.notify_one();
					}
					
					});

				runningProcesses[process->getProcessName()].detach();
			}
		}

	}
}