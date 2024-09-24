#include "ScreenSession.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

using namespace std;

static int sessionCounter = 0; // Global session counter to assign unique IDs

ScreenSession::ScreenSession(string name, int totalLines)
    : processName(name), sessionId(++sessionCounter), currentLine(0), totalLines(totalLines), isActive(true) {
    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now); // Use localtime_s instead of localtime
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "%m/%d/%Y, %I:%M:%S %p", &localtm);
    creationTimestamp = string(timestamp);
}

void ScreenSession::displayDetails() const {
    cout << endl << "   Process: " << processName << endl;
    cout << "   ID: " << sessionId << endl;
    cout << "   Current instruction line: " << currentLine << " / " << totalLines << endl;
    cout << "   Created at: " << creationTimestamp << endl << endl;
}

void ScreenSession::run() {
    while (isActive && currentLine < totalLines) {
        this_thread::sleep_for(chrono::seconds(1));  // Simulate a process running
        currentLine++;
    }
}

void ScreenSession::stop() {
    isActive = false;
}