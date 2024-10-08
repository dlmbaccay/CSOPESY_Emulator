#include "ScreenManager.h"
#include "Colors.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

static bool inSession = false;

static string currentSessionName = ""; // Track current session name

static ScreenManager screenManager(4);

static void printHeader();
static bool handleCommand(const string& command);

int main() {
    for (int i = 1; i <= 10; ++i) {
        screenManager.createProcess("p" + to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(40));  // 1-second delay between each process creation
    }

    system("cls");
    
    string input;
    printHeader();  // Display the initial header
    bool running = true;

    
    while (running) {
        // If in a session, show the session name in the prompt
        if (inSession) {
            cout << "[" << currentSessionName << "] " << "> ";
        }
        else {
            cout << "> ";
        }

        // Get the user input
        getline(cin, input);

        // Handle the command and check if the application should keep running
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

    // Handle 'exit' when the user is inside a session
    if (inSession && command == "exit") {
        cout << "> Exiting session..." << endl;
        inSession = false;  // Mark as no longer in a session
        currentSessionName = "";
        system("cls");  // Clear the screen when returning to the main menu
        printHeader();  // Re-display the main menu header
        return true;
    }

    // If not in a session, process general commands
    if (!inSession) {
		if (command == "") {
			// If the user presses Enter without typing anything
			return true;
		}
        else if (command == "initialize") {
            // Command to initialize something
            cout << GREEN << "> initialize command recognized. Doing something." << RESET << endl;
        }
        else if (command.substr(0, 9) == "screen -s") {
			// Start a new screen session
            
            // if input was screen -s <blank>
            if (command.length() <= 10 || command.substr(10).find_first_not_of(' ') == string::npos) {
                // If there's no input after "screen -s" or it's just whitespace
                cout << RED << "> Error: Missing process name for 'screen -s' command." << RESET << endl;
            }
            else {
                // Create a new screen session with the given name
                string processName = command.substr(10);
                screenManager.createProcess(processName);  // Create a session
                inSession = true;  // Mark that the user is now in a session
                currentSessionName = processName;
            }
        }
        else if (command.substr(0, 9) == "screen -r") {
            // Reattach to an existing session

            if (command.length() <= 10 || command.substr(10).find_first_not_of(' ') == string::npos) {
                // If there's no input after "screen -r" or it's just whitespace
                cout << RED << "> Error: Missing process name for 'screen -r' command." << RESET << endl;
            }
            else {
                string processName = command.substr(10);
                if (screenManager.reattachProcess(processName)) {
                    inSession = true;  // Mark that the user is now in a session
                    currentSessionName = processName;  // Set the active session name
                }
            }
        }
        else if (command == "screen -ls") {
            // List all active sessions
            system("cls");
            printHeader(); 
            screenManager.listProcess();
        }
        else if (command == "scheduler-test") {
            // Handle scheduler-test command
            cout << GREEN << "> scheduler-test command recognized. Doing something." << RESET << endl;
        }
        else if (command == "scheduler-stop") {
            // Handle scheduler-stop command
            cout << GREEN << "> scheduler-stop command recognized. Doing something." << RESET << endl;
        }
        else if (command == "report-util") {
            // Handle report-util command
            cout << GREEN << "> report-util command recognized. Doing something." << RESET << endl;
        }
        else if (command == "clear") {
            // Clear the screen and print the header again
            system("cls");
            printHeader();
        }
        else if (command == "help") {
            // Display the list of available commands
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
            // Exit the emulator
            cout << "> Exiting emulator..." << endl;
            exit(0);
        }
        else {
            // Handle unrecognized command
            cout << "> Unrecognized command: " << command << endl;
        }
    }
    else {
        // If the user is in a session and tries an invalid command
        cout << "> You are in session. Use 'exit' to leave." << endl;
    }

    return true;
}