#include "ConsoleManager.h"
#include <iostream>
#include <thread>
#include <iomanip>
#include <fstream>
#include <format>
#include <string>
#include <map>
#include <memory>
#include <mutex>

using namespace std;

ConsoleManager::ConsoleManager(): configManager(new ConfigManager()) {
	scheduler = new Scheduler(configManager);
}

ConsoleManager::~ConsoleManager() {
    for (auto& process : processes) {
        delete process.second;
    }
}

bool ConsoleManager::createProcess(const std::string& name) {
    lock_guard<mutex> lock(processMutex);

    if (processes.find(name) == processes.end()) {
        Process* process = new Process(name, configManager->getMinIns(), configManager->getMaxIns());
        processes[name] = process;
        scheduler->addProcess(process);  // Add process to scheduler
        return true;
    }
    else {
        cout << "> Process " << name << " already exists." << endl;
        return false;
    }
}

bool ConsoleManager::reattachProcess(const std::string& name) {
    lock_guard<mutex> lock(processMutex);

    if (processes.find(name) != processes.end()) {
        Process* process = processes[name];
        cout << "> Reattached to process: " << name << endl;
        process->displayDetails();
        return true;
    }
    else {
        cout << "> No process found for: " << name << endl;
        return false;
    }
}

void ConsoleManager::listProcess() {
    lock_guard<mutex> lock(processMutex);

    if (processes.empty()) {
        cout << "> No processes available." << endl;
        return;
    }

    bool hasRunning = false, hasFinished = false;

    scheduler->displayCpuUtilization();

    // Display running processes
    cout << "---------------------------------------" << endl;
    cout << "Running processes:" << endl;
    for (const auto& pair : processes) {
        if (pair.second->getStatus() == Process::RUNNING) {
            hasRunning = true;
            cout << format("{:>8}   {}   Core: {}   {} / {}\n",
                pair.first, pair.second->getRunTimestamp(), pair.second->getCoreIndex(),
                pair.second->getCommandIndex() + 1, pair.second->getTotalCommands());
        }
    }
    if (!hasRunning) {
        cout << "   No running processes." << endl;
    }

    // Display finished processes
    cout << endl;
    cout << "Finished processes:" << endl;
    for (const auto& pair : processes) {
        if (pair.second->getStatus() == Process::FINISHED) {
            hasFinished = true;
            cout << format("{:>8}   {}   Finished   {} / {}\n",
                pair.first, pair.second->getRunTimestamp(),
                pair.second->getCommandIndex()+1, pair.second->getTotalCommands());
        }
    }
    if (!hasFinished) {
        cout << "   No finished processes." << endl;
    }
    cout << "---------------------------------------" << endl << endl;
}

void ConsoleManager::schedulerTest() {
    std::thread([this]{
        schedulerTestRun = true;
        
        int cpuCycles = 1;
		int i = 1;

        while (schedulerTestRun) {
            if (cpuCycles % configManager->getBatchProcessFreq() == 0) {
                string processName = "process" + to_string(i);
                createProcess(processName);
                i++;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            cpuCycles++;
        }
     }).detach();
    
}

void ConsoleManager::reportUtil() {
    // Write to a text file csopesy-log.txt
    string fileName = "csopesy-log.txt";  // Generate file name based on process name
    ofstream outFile(fileName, ios::out | ios::trunc);  // Open the file for writing and trunc if not empty

    if (outFile.is_open()){
        if (processes.empty()) {
            outFile << "> No processes available." << endl;
            return;
        }

        bool hasRunning = false, hasFinished = false;

		// TODO: Print out cpu utilization to text file. scheduler->displayCpuUtilization();
		Scheduler::CpuUtilization util = scheduler->getCpuUtilization();

        outFile << "CPU utilization: " << util.utilization << "%" << endl;
        outFile << "Cores used: " << util.usedCores << endl;
        outFile << "Cores available: " << util.availableCores << endl << endl;

        // Display queued processes
        outFile << "---------------------------------------" << endl;
        outFile << "Running processes:" << endl;
        for (const auto& pair : processes) {
            if (pair.second->getStatus() == Process::RUNNING) {
                hasRunning = true;
                outFile << format("{:>8}   {}   Core: {}   {} / {}\n",
                    pair.first, pair.second->getRunTimestamp(), pair.second->getCoreIndex(),
                    pair.second->getCommandIndex() + 1, pair.second->getTotalCommands());
            }
        }
        if (!hasRunning) {
            outFile << "   No running processes." << endl;
        }

        // Display finished processes
        outFile << endl;
        outFile << "Finished processes:" << endl;
        for (const auto& pair : processes) {
            if (pair.second->getStatus() == Process::FINISHED) {
                hasFinished = true;
                outFile << format("{:>8}   {}   Finished   {} / {}\n",
                    pair.first, pair.second->getRunTimestamp(),
                    pair.second->getCommandIndex() + 1, pair.second->getTotalCommands());
            }
        }
        if (!hasFinished) {
            outFile << "   No finished processes." << endl;
        }
        outFile << "---------------------------------------" << endl << endl;
        outFile.close();
    }

    // Display path to file
	cout << "> Report generated at " << fileName << "!" << endl;
    
}