#include "Process.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <fstream>
#include <random>

using namespace std;

static int processCounter = 0; // Global process counter to assign unique IDs

Process::Process(string name, int minCommands, int maxCommands)
    : processName(name), processId(++processCounter), commandIndex(0), isActive(true), status(READY), coreIndex(-1), runTimestamp() {
    
    // Seed the random number generator
    random_device rd;
    mt19937 gen(rd()); // Mersenne Twister engine for randomness
    uniform_int_distribution<> dist(minCommands, maxCommands);

    // Generate a random number of commands between minCommands and maxCommands
    int numCommands = dist(gen);

    // Fill the commands vector with "dummy instruction" strings
    for (int i = 1; i <= numCommands; ++i) {
        commands.push_back("dummy instruction " + to_string(i));
    }

    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now);  // Use thread-safe localtime_s
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "(%m/%d/%Y %I:%M:%S%p)", &localtm);
	creationTimestamp = string(timestamp);  // Store the formatted timestamp
}

void Process::displayDetails() const {
    string statusString;
    switch (status) {
	    case WAITING: statusString = "Waiting"; break;
        case READY: statusString = "Ready"; break;
        case RUNNING: statusString = "Running"; break;
        case FINISHED: statusString = "Finished"; break;
    }

    cout << endl << "   Process: " << processName << endl;
    cout << "   ID: " << processId << endl;
    cout << "   Core: " << coreIndex << endl;
    cout << "   Current instruction line: " << commandIndex << " / " << commands.size() << endl;
    cout << "   Run at: " << runTimestamp << endl;
    cout << "   Status: " << statusString << endl << endl;
}

void Process::setTimestamp() {
    // Capture the current time as the start timestamp
    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now);  // Use thread-safe localtime_s
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "(%m/%d/%Y %I:%M:%S%p)", &localtm);
    runTimestamp = string(timestamp);  // Store the formatted timestamp
}

void Process::getNextCommand() {
    if (commandIndex < commands.size()-1) {
        commandIndex++;
    }
    else {
        setStatus(FINISHED);
    }
}

void Process::execute() {
    // commands[commandIndex];
}

void Process::processSMI() {
    cout << endl;
    cout << "Process: " << processName << endl;
    cout << "ID: " << processId << endl << endl;
    
    if (status == FINISHED) {
        cout << "Finished!" << endl << endl;
    }
    else {
        cout << "Current instruction line: " << commandIndex+1 << endl;
        cout << "Lines of code: " << commands.size() << endl << endl;
    }

}