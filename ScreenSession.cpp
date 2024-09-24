#include "ScreenSession.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

using namespace std;

static int sessionCounter = 0; // Global session counter to assign unique IDs

/**
* Constructs a new ScreenSession instance.
*
* Initializes a new session with a given name and the total number of lines. The session
* is assigned a unique ID and sets its creation timestamp.
*
* @param name The name of the process to be associated with the session.
* @param totalLines The total number of lines (steps) the session will execute.
*/
ScreenSession::ScreenSession(string name, int totalLines)
    : processName(name), sessionId(++sessionCounter), currentLine(0), totalLines(totalLines), isActive(true) {
    // Capture the current time as the creation timestamp
    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now); // Use localtime_s instead of localtime for thread safety
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "%m/%d/%Y, %I:%M:%S %p", &localtm);
    creationTimestamp = string(timestamp);  // Store the formatted timestamp
}

/**
* Displays the details of the session.
*
* This method outputs the process name, session ID, current instruction line, total lines,
* and the timestamp when the session was created.
*/
void ScreenSession::displayDetails() const {
    cout << endl << "   Process: " << processName << endl;
    cout << "   ID: " << sessionId << endl;
    cout << "   Current instruction line: " << currentLine << " / " << totalLines << endl;
    cout << "   Created at: " << creationTimestamp << endl << endl;
}

/**
* Simulates the running of the session.
*
* This method runs the session, incrementing the current instruction line every second.
* The session continues until it reaches the total lines or is stopped.
*/
void ScreenSession::run() {
    // Run the session, simulating progress in instruction lines
    while (isActive && currentLine < totalLines) {
        this_thread::sleep_for(chrono::seconds(1));  // Simulate a process running
        currentLine++;  // Increment the current line
    }
}