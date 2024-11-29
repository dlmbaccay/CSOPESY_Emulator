#include "ConfigManager.h"
#include "Colors.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

ConfigManager::ConfigManager()
{
	numCpu = 0;
	schedulerType = SchedulerType::FCFS;
	quantumCycles = 0;
	batchProcessFreq = 0;
	minIns = 0;
	maxIns = 0;
	delayPerExec = 0;
	maxOverallMem = 0;
	memPerFrame = 0;
	minMemPerProcess = 0;
	maxMemPerProcess = 0;
	readConfig();
}

ConfigManager::~ConfigManager()
{
}

void ConfigManager::readConfig() {
    std::ifstream file("config.txt");
    std::string line;

    if (!file.is_open()) {
        cout << RED << "> Error: Could not open the file 'config.txt'." << RESET << endl;
        return;
    }
    while (std::getline(file, line)) {
        if (line.find("num-cpu") == 0) {
            numCpu = stoi(line.substr(8));  // Parse num-cpu value
        }
        else if (line.find("scheduler") == 0) {
            std::string typeString = line.substr(10);  // Parse scheduler value
            if (typeString == "'fcfs'") {
                schedulerType = SchedulerType::FCFS;
            }
            else if (typeString == "'rr'") {
                schedulerType = SchedulerType::RR;
            }
        }
        else if (line.find("quantum-cycles") == 0) {
            quantumCycles = stoi(line.substr(15));  // Parse quantum-cycles value
        }
        else if (line.find("batch-process-freq") == 0) {
            batchProcessFreq = stoi(line.substr(19));  // Parse batch-process-freq value
        }
        else if (line.find("min-ins") == 0) {
            minIns = stoi(line.substr(8));  // Parse min-ins value
        }
        else if (line.find("max-ins") == 0) {
            maxIns = stoi(line.substr(8));  // Parse max-ins value
        }
        else if (line.find("delay-per-exec") == 0) {
            delayPerExec = stoi(line.substr(15));  // Parse delay-per-exec value
        }
				else if (line.find("max-overall-mem") == 0) {
					maxOverallMem = stoi(line.substr(16));  // Parse max-overall-mem value
				}
				else if (line.find("mem-per-frame") == 0) {
					memPerFrame = stoi(line.substr(14));  // Parse mem-per-frame value
				}
				else if (line.find("min-mem-per-proc") == 0) {
					minMemPerProcess = stoi(line.substr(17));  // Parse mem-per-process value
				}
        else if (line.find("max-mem-per-proc") == 0) {
          maxMemPerProcess = stoi(line.substr(17));  // Parse mem-per-process value
        }

    }

    file.close();

	 // displayConfig();
}

void ConfigManager::displayConfig() {
	std::cout << "Number of CPUs: " << numCpu << std::endl;
    std::cout << "Scheduler type: ";
    switch (schedulerType) {
        case SchedulerType::FCFS:
            std::cout << "FCFS" << std::endl;
            break;
        case SchedulerType::RR:
            std::cout << "RR" << std::endl;
            break;
        default:
            std::cout << "Unknown" << std::endl;
            break;
        }
	std::cout << "Quantum cycles: " << quantumCycles << std::endl;
	std::cout << "Batch process frequency: " << batchProcessFreq << std::endl;
	std::cout << "Minimum instructions: " << minIns << std::endl;
	std::cout << "Maximum instructions: " << maxIns << std::endl;
	std::cout << "Delay per execution: " << delayPerExec << std::endl;
	std::cout << "Max overall memory: " << maxOverallMem << std::endl;
	std::cout << "Memory per frame: " << memPerFrame << std::endl;
	std::cout << "Min Memory per process: " << minMemPerProcess << std::endl;
	std::cout << "Max Memory per process: " << maxMemPerProcess << std::endl;
}