#include "Process.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <fstream>

using namespace std;

static int processCounter = 0; // Global process counter to assign unique IDs

Process::Process(string name, int totalLines)
    : processName(name), processId(++processCounter), currentLine(0), totalLines(totalLines), isActive(true), status(Queued), coreIndex(-1), runTimestamp() {
}

void Process::displayDetails() const {
    string statusString;
    switch (status) {
        case Queued: statusString = "Queued"; break;
        case Running: statusString = "Running"; break;
        case Finished: statusString = "Finished"; break;
    }

    cout << endl << "   Process: " << processName << endl;
    cout << "   ID: " << processId << endl;
    cout << "   Core:" << coreIndex << endl;
    cout << "   Current instruction line: " << currentLine << " / " << totalLines << endl;
    cout << "   Run at: " << runTimestamp << endl;
    cout << "   Status: " << statusString << endl << endl;
}

void Process::run(int coreIndex) {
    this->coreIndex = coreIndex;  // Set the CPU core index
    setTimestamp();  // Set the timestamp when the process starts running
    status = Running;  // Mark the process as running

    // Create a text file to store process information
    string fileName = processName + "_log.txt";  // Generate file name based on process name
    ofstream outFile(fileName, ios::out | ios::trunc);  // Open the file for writing and trunc if not empty

    if (outFile.is_open()) {
        // Write the initial process details into the file
        outFile << "Process Name: " << processName << endl;
        outFile << "Logs: " << endl << endl;
    }
    else {
        cerr << "Error: Unable to open file for writing process details." << endl;
    }

    // Simulate process execution
    while (isActive && currentLine < totalLines) {
        this_thread::sleep_for(chrono::milliseconds(20));  // Simulate a process running
        currentLine++;

        // Get the current timestamp for logging progress
        time_t now = time(0);
        tm localtm;
        localtime_s(&localtm, &now);  // Use thread-safe localtime_s
        char timestamp[100];
        strftime(timestamp, sizeof(timestamp), "%m/%d/%Y, %I:%M:%S %p", &localtm);
        string currentTimestamp = string(timestamp);

        // Log the current progress to the text file
        if (outFile.is_open()) {
            outFile << "(" << runTimestamp << ")" << " Core: " << coreIndex 
                << " \"Hello world from " << processName << "!\"" << endl;
        }
    }

    // Update the process status to finished in the log file
    if (outFile.is_open()) {
        outFile << endl << "Process " << processName << " has finished execution." << endl;
        outFile.close();  // Close the file after logging completion
    }

    status = Finished;  // Mark the process as finished when done
}

void Process::setTimestamp() {
    // Capture the current time as the start timestamp
    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now);  // Use thread-safe localtime_s
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "%m/%d/%Y, %I:%M:%S %p", &localtm);
    runTimestamp = string(timestamp);  // Store the formatted timestamp
}
