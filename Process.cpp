#include "Process.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

using namespace std;

static int processCounter = 0; // Global process counter to assign unique IDs

/**
* Constructs a new Process instance.
*
* Initializes a new process with a given name and the total number of lines. The process
* is assigned a unique ID and sets its creation timestamp.
*
* @param name The name of the process.
* @param totalLines The total number of lines (steps) the process will execute.
*/
Process::Process(string name, int totalLines)
    : processName(name), processId(++processCounter), currentLine(0), totalLines(totalLines), isActive(true) {
    // Capture the current time as the creation timestamp
    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now); // Use localtime_s instead of localtime for thread safety
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "%m/%d/%Y, %I:%M:%S %p", &localtm);
    creationTimestamp = string(timestamp);  // Store the formatted timestamp
}

/**
* Displays the details of the process.
*
* This method outputs the process name, process ID, current instruction line, total lines,
* and the timestamp when the process was created.
*/
void Process::displayDetails() const {
    cout << endl << "   Process: " << processName << endl;
    cout << "   ID: " << processId << endl;
    cout << "   Current instruction line: " << currentLine << " / " << totalLines << endl;
    cout << "   Created at: " << creationTimestamp << endl << endl;
}

/**
* Simulates the running of the process.
*
* This method runs the process, incrementing the current instruction line every second.
* The process continues until it reaches the total lines or is stopped.
*/
void Process::run() {
    // Run the process, simulating progress in instruction lines
    while (isActive && currentLine < totalLines) {
        this_thread::sleep_for(chrono::seconds(1));  // Simulate a process running
        currentLine++;  // Increment the current line
    }
}
