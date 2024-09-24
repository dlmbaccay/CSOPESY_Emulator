#include "ScreenManager.h"
#include <iostream>
#include <thread>
#include <cstdlib>

using namespace std;

/**
 * Destructor for ScreenManager.
 *
 * This destructor ensures that all dynamically allocated ScreenSession instances are deleted
 * when the ScreenManager instance is destroyed.
 */
ScreenManager::~ScreenManager() {
    for (auto& session : sessions) {
        delete session.second;
    }
}

/**
 * Creates a new screen session if one does not already exist.
 *
 * If a session with the provided name does not already exist, this method creates a new ScreenSession instance.
 * It defaults the session to having 150 total lines and runs the session in a separate thread.
 * If the session already exists, it displays a message and ports the user into the existing session.
 *
 * @param name The name of the session to create.
 */
void ScreenManager::createSession(const string& name) {
    lock_guard<mutex> lock(sessionMutex); // Ensure thread safety

    int totalLines = 150;  // Default total lines for each session

    if (sessions.find(name) == sessions.end()) {
        // If no session with the name exists, create a new session
        ScreenSession* session = new ScreenSession(name, totalLines);
        sessions[name] = session;
        thread(&ScreenSession::run, session).detach();  // Run the session in a separate thread
        system("cls");  // Clear the screen when the session is created
        cout << "> Created session for process: " << name << endl;
        session->displayDetails();  // Display session details
    }
    else {
        // If session already exists, port the user into that session
        system("cls");  // Clear the screen when re-entering the session
        cout << "> Session for process " << name << " already exists." << endl;
        ScreenSession* session = sessions[name];
        session->displayDetails();  // Redisplay session details
    }
}

/**
* Reattaches the user to an existing session.
*
* If a session with the provided name exists, this method reattaches the user to that session,
* clears the screen, and redisplays the session details. If no session exists with that name,
* it returns false.
*
* @param name The name of the session to reattach to.
* @return true if the session was successfully reattached, false otherwise.
*/
bool ScreenManager::reattachSession(const string& name) {
    lock_guard<mutex> lock(sessionMutex); // Ensure thread safety

    if (sessions.find(name) != sessions.end()) {
        // If the session exists, reattach to it
        ScreenSession* session = sessions[name];
        system("cls");  // Clear the screen when reattaching
        cout << "> Reattached to session: " << name << endl;
        session->displayDetails();  // Display session details
        return true;
    }
    else {
        // If no session with the name exists, return false
        cout << "> No session found for process: " << name << endl;
        return false;
    }
}

/**
* Lists all active screen sessions.
*
* This method displays a list of all currently active screen sessions, showing the session name,
* session ID, current instruction line, total lines, and the timestamp of when the session was created.
* If no sessions are active, it displays a message indicating that.
*/
void ScreenManager::listSessions() {
    lock_guard<mutex> lock(sessionMutex); // Ensure thread safety

    if (sessions.empty()) {
        cout << "> No active sessions." << endl;  // No active sessions to display
    }
    else {
        cout << "> Active sessions:" << endl;  // Display active sessions
        for (const auto& pair : sessions) {
            cout << " - " << pair.first << " | " << "ID: " << pair.second->sessionId
                << " | " << pair.second->currentLine << "/" << pair.second->totalLines
                << " | " << pair.second->creationTimestamp << endl;
        }
    }
}