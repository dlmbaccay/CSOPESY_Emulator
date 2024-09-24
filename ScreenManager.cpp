#include "ScreenManager.h"
#include <iostream>
#include <thread>
#include <cstdlib>

using namespace std;

ScreenManager::~ScreenManager() {
    for (auto& session : sessions) {
        delete session.second;
    }
}

void ScreenManager::createSession(const string& name) {
    lock_guard<mutex> lock(sessionMutex);
    int totalLines = 150;

    if (sessions.find(name) == sessions.end()) {
        // Create a new session
        ScreenSession* session = new ScreenSession(name, totalLines);
        sessions[name] = session;
        thread(&ScreenSession::run, session).detach();  // Run the session in a separate thread
        system("cls");  // Clear screen when session is created
        cout << "> Created session for process: " << name << endl;
        session->displayDetails();
    }
    else {
        // If session already exists, port the user into that session
        system("cls");  // Clear the screen when re-entering the session
        cout << "> Session for process " << name << " already exists." << endl;
        ScreenSession* session = sessions[name];
        session->displayDetails();  // Redisplay session details
    }
}

bool ScreenManager::reattachSession(const string& name) {
    lock_guard<mutex> lock(sessionMutex);
    if (sessions.find(name) != sessions.end()) {
        ScreenSession* session = sessions[name];
        system("cls");  // Clear screen when session is reattached
        cout << "> Reattached to session: " << name << endl;
        session->displayDetails();
        return true;
    }
    else {
        cout << "> No session found for process: " << name << endl;
        return false;
    }
}

void ScreenManager::listSessions() {
    lock_guard<mutex> lock(sessionMutex);
    if (sessions.empty()) {
        cout << "> No active sessions." << endl;
    }
    else {
        cout << "> Active sessions:" << endl;
        for (const auto& pair : sessions) {
            cout << " - " << pair.first << " | " << "ID: " << pair.second->sessionId << " | " << pair.second->currentLine << "/" << pair.second->totalLines << " | " << pair.second->creationTimestamp << endl;
        }
    }
}