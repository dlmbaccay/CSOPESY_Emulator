#include "ScreenManager.h"
#include "Colors.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

static bool inSession = false;
static string currentSessionName = ""; // Track current session name
static ScreenManager screenManager;

static void printHeader();
static bool handleCommand(const string& command);

int main() {
    string input;
    printHeader();
    bool running = true;
    while (running) {
        if (inSession) {
            cout << "[" << currentSessionName << "] " << "> ";
        }
        else {
            cout << "> ";
        }
        getline(cin, input);
        running = handleCommand(input);
    }
    return 0;
}

static void printHeader() {
    cout << GREEN << "   _   _  _ ___ __  __ " << WHITE << " ___  ___  " << endl
        << GREEN << "  /_\\ | \\| |_ _|  \\/  |" << WHITE << "/ _ \\/ __| " << endl
        << GREEN << " / _ \\| .` || || |\\/| |" << WHITE << " (_) \\__ \\ " << endl
        << GREEN << "/_/ \\_\\_|\\_|___|_|  |_|" << WHITE << "\\___/|___/ " << endl
        << GREEN << "\nWelcome to ANIMOS Command Line Emulator!" << endl
        << LIGHT_YELLOW << "\nType 'help' to view all commands\n" << RESET << endl;
}

static bool handleCommand(const string& command) {
    if (inSession && command == "exit") {
        // Exiting from the session, return to main menu
        cout << "> Exiting session..." << endl;
        inSession = false;
        currentSessionName = "";
        system("cls");  // Clear screen when returning to main menu
        printHeader();  // Print the main menu header again
        return true;
    }

    if (!inSession) {
        if (command == "initialize") {
            cout << GREEN << "> initialize command recognized. Doing something." << RESET << endl;
        }
        else if (command.substr(0, 9) == "screen -s") {
            string processName = command.substr(10);
            screenManager.createSession(processName);  // Example with 100 lines
            inSession = true;
            currentSessionName = processName;
        }
        else if (command.substr(0, 9) == "screen -r") {
            string processName = command.substr(10);
            if (screenManager.reattachSession(processName)) {
                inSession = true;
                currentSessionName = processName;  // Track current session name
            }
        }
        else if (command == "screen -ls") {
            screenManager.listSessions();
        }
        else if (command == "scheduler-test")
        {
            cout << GREEN << "> scheduler-test command recognized. Doing something." << RESET << endl;
        }
        else if (command == "scheduler-stop")
        {
            cout << GREEN << "> scheduler-stop command recognized. Doing something." << RESET << endl;
        }
        else if (command == "report-util")
        {
            cout << GREEN << "> report-util command recognized. Doing something." << RESET << endl;
        }
        else if (command == "clear") {
            system("cls");
            printHeader();
        }
        else if (command == "help") {
            cout << "> List of commands:" << endl
                << "    - initialize            (...)" << endl
                << "    - screen -s <name>      (start a session)" << endl
                << "    - screen -r <name>      (reattach to a session)" << endl
                << "    - screen -ls            (list all sessions)" << endl
                << "    - scheduler-test        (...)" << endl
                << "    - scheduler-stop        (...)" << endl
                << "    - report-util           (...)" << endl
                << "    - clear                 (clear the screen)" << endl
                << "    - exit                  (exit the emulator)" << endl;
        }
        else if (command == "exit") {
            cout << "> Exiting emulator..." << endl;
            exit(0);
        }
        else {
            cout << "> Unrecognized command: " << command << endl;
        }
    }
    else {
        cout << "> You are in session. Use 'exit' to leave." << endl;
    }

    return true;
}