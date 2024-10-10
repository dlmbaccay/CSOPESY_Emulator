#include "ScreenManager.h"
#include <iostream>
#include <thread>
#include <iomanip>
#include <format>
#include <string>
#include <map>
#include <memory>
#include <mutex>

using namespace std;

ScreenManager::ScreenManager(int cores) : coreCount(cores), availableCores(cores) {
    cpuCores = vector<bool>(cores, false);
    startScheduler();  // Start the scheduler when the ScreenManager is initialized
}

ScreenManager::~ScreenManager() {
    for (auto& process : processes) {
        delete process.second;
    }
}

void ScreenManager::createProcess(const std::string& name) {
    lock_guard<mutex> lock(processMutex);

    int totalLines = 100;  // Default total lines for each process
    if (processes.find(name) == processes.end()) {
        Process* process = new Process(name, totalLines);
        processes[name] = process;
        waitingQueue.push(process);  // Add to waiting queue

        cout << "> Created process: " << name << endl;
        process->displayDetails();
    }
    else {
        cout << "> Process " << name << " already exists." << endl;
    }
}

bool ScreenManager::reattachProcess(const std::string& name) {
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

void ScreenManager::listProcess() {
    lock_guard<mutex> lock(processMutex);

    if (processes.empty()) {
        cout << "> No processes available." << endl;
        return;
    }

    bool hasQueued = false, hasRunning = false, hasFinished = false;

    // Display queued processes
    cout << "---------------------------------------" << endl;
    cout << "Queued processes:" << endl;
    for (const auto& pair : processes) {
        if (pair.second->status == Process::Queued) {
            hasQueued = true;
            cout << format("{:>8}   {} / {}\n", pair.first, pair.second->currentLine, pair.second->totalLines);
        }
    }
    if (!hasQueued) {
        cout << "   No queued processes." << endl;
    }

    // Display running processes
    cout << endl;
    cout << "Running processes:" << endl;
    for (const auto& pair : processes) {
        if (pair.second->status == Process::Running) {
            hasRunning = true;
            cout << format("{:>8}   {}   Core: {}   {} / {}\n",
                pair.first, pair.second->runTimestamp, pair.second->coreIndex,
                pair.second->currentLine, pair.second->totalLines);
        }
    }
    if (!hasRunning) {
        cout << "   No running processes." << endl;
    }

    // Display finished processes
    cout << endl;
    cout << "Finished processes:" << endl;
    for (const auto& pair : processes) {
        if (pair.second->status == Process::Finished) {
            hasFinished = true;
            cout << format("{:>8}   {}   Finished   {} / {}\n",
                pair.first, pair.second->runTimestamp,
                pair.second->currentLine, pair.second->totalLines);
        }
    }
    if (!hasFinished) {
        cout << "   No finished processes." << endl;
    }
    cout << "---------------------------------------" << endl;

}



void ScreenManager::startScheduler() {
    std::thread schedulerThread(&ScreenManager::scheduleProcesses, this);
    schedulerThread.detach();  // Detach the scheduler thread to run in the background
}

void ScreenManager::scheduleProcesses() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Small delay to prevent tight looping

        lock_guard<mutex> lock(processMutex);

        // If there are available cores and processes in the waiting queue
        for (int i = 0; i < cpuCores.size(); ++i) {
            if (!cpuCores[i] && !waitingQueue.empty()) {  // Check for a free core
                Process* nextProcess = waitingQueue.front();
                waitingQueue.pop();

                cpuCores[i] = true;  // Mark the core as occupied

                runningProcesses[nextProcess->processName] = std::thread([this, nextProcess, i]() {
                    nextProcess->run(i);  // Pass the core index to the process
                    lock_guard<mutex> lock(processMutex);
                    cpuCores[i] = false;  // Release the core when the process finishes
                    });

                runningProcesses[nextProcess->processName].detach();  // Detach the process thread
            }
        }
    }
}
