#include "ScreenManager.h"
#include <iostream>
#include <thread>
#include <cstdlib>

using namespace std;

/**
 * Destructor for ScreenManager.
 *
 * This destructor ensures that all dynamically allocated Process instances are deleted
 * when the ScreenManager instance is destroyed.
 */
ScreenManager::~ScreenManager() {
    for (auto& process : processes) {
        delete process.second;
    }
}

/**
 * Creates a new process if one does not already exist.
 *
 * If a process with the provided name does not already exist, this method creates a new Process instance.
 * It defaults the process to having 150 total lines and runs the process in a separate thread.
 * If the process already exists, it displays a message and ports the user into the existing process.
 *
 * @param name The name of the process to create.
 */
void ScreenManager::createProcess(const string& name) {
    lock_guard<mutex> lock(processMutex); // Ensure thread safety

    int totalLines = 150;  // Default total lines for each process

    if (processes.find(name) == processes.end()) {
        // If no process with the name exists, create a new process
        Process* process = new Process(name, totalLines);
        processes[name] = process;
        thread(&Process::run, process).detach();  // Run the process in a separate thread
        system("cls");  // Clear the screen when the process is created
        cout << "> Created process: " << name << endl;
        process->displayDetails();  // Display process details
    }
    else {
        // If process already exists, port the user into that process
        system("cls");  // Clear the screen when re-entering the process
        cout << "> Process " << name << " already exists." << endl;
        Process* process = processes[name];
        process->displayDetails();  // Redisplay process details
    }
}

/**
* Reattaches the user to an existing process.
*
* If a process with the provided name exists, this method reattaches the user to that process,
* clears the screen, and redisplays the process details. If no process exists with that name,
* it returns false.
*
* @param name The name of the process to reattach to.
* @return true if the process was successfully reattached, false otherwise.
*/
bool ScreenManager::reattachProcess(const string& name) {
    lock_guard<mutex> lock(processMutex); // Ensure thread safety

    if (processes.find(name) != processes.end()) {
        // If the process exists, reattach to it
        Process* process = processes[name];
        system("cls");  // Clear the screen when reattaching
        cout << "> Reattached to process: " << name << endl;
        process->displayDetails();  // Display process details
        return true;
    }
    else {
        // If no process with the name exists, return false
        cout << "> No process found for: " << name << endl;
        return false;
    }
}

/**
* Lists all active processes.
*
* This method displays a list of all currently active processes, showing the process name,
* process ID, current instruction line, total lines, and the timestamp of when the process was created.
* If no processes are active, it displays a message indicating that.
*/
void ScreenManager::listProcess() {
    lock_guard<mutex> lock(processMutex); // Ensure thread safety

    if (processes.empty()) {
        cout << "> No active processes." << endl;  // No active processes to display
    }
    else {
        cout << "> Active processes:" << endl;  // Display active processes
        for (const auto& pair : processes) {
            cout << " - " << pair.first << " | " << "ID: " << pair.second->processId
                << " | " << pair.second->currentLine << "/" << pair.second->totalLines
                << " | " << pair.second->creationTimestamp << endl;
        }
    }
}
