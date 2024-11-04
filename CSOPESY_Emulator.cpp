#include "Colors.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "ConsoleManager.h"

using namespace std;

static ConsoleManager* consoleManager;

int main() {
    int cpuCycles = 0;
    string input;
    bool running = true;
    consoleManager = new ConsoleManager();
    consoleManager->printHeader();  // Display the initial header
    
    while (running) {
        // If in a session, show the session name in the prompt
        if (consoleManager->isInSession()) {
            cout << "root:\\" << consoleManager->getCurrentSessionName() << "\\" << "> ";
        }
        else {
            cout << "root:\\> ";
        }

        // Get the user input
        getline(cin, input);

        // Handle the command and check if the application should keep running
        running = consoleManager->handleCommand(input);
    }

    return 0;
}
