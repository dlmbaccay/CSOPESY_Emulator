#include "ConsoleManager.h"
#include <iostream>
#include <thread>
#include <iomanip>
#include <fstream>
#include <format>
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include "Colors.h"

using namespace std;

ConsoleManager::ConsoleManager() {
}

ConsoleManager::~ConsoleManager() {
    for (auto& process : processes) {
        delete process.second;
    }
}

bool ConsoleManager::createProcess(const std::string& name) {
    lock_guard<mutex> lock(processMutex);

    if (processes.find(name) == processes.end()) {
        Process* process = new Process(name, configManager->getMinIns(), configManager->getMaxIns(), configManager->getMinMemPerProcess(), configManager->getMaxMemPerProcess(), configManager->getMemPerFrame());
        processes[name] = process;
        scheduler->addProcess(process);  // Add process to scheduler
        return true;
    }
    else {
        cout << "> Process " << name << " already exists." << endl;
        return false;
    }
}

bool ConsoleManager::reattachProcess(const std::string& name) {
    lock_guard<mutex> lock(processMutex);

    if (processes.find(name) != processes.end()) {
        Process* process = processes[name];
        return true;
    }
    else {
        cout << "> No process found for: " << name << endl;
        return false;
    }
}

void ConsoleManager::listProcess() {
    lock_guard<mutex> lock(processMutex);

    bool hasRunning = false, hasFinished = false;

    scheduler->displayCpuUtilization();

    // Display running processes
    cout << "---------------------------------------" << endl;
    cout << "Running processes:" << endl;
    for (const auto& pair : processes) {
        if (pair.second->getStatus() == Process::RUNNING) {
            hasRunning = true;
            cout << format("{:>8}   {}   Core: {}   {} / {}\n",
                pair.first, pair.second->getRunTimestamp(), pair.second->getCoreIndex(),
                pair.second->getCommandIndex() + 1, pair.second->getTotalCommands());
        }
    }
    if (!hasRunning) {
        cout << "   No running processes." << endl;
    }

    // Display finished processes
    cout << endl;
    cout << "Finished processes:" << endl;
    for (const auto& pair : processes) {
        if (pair.second->getStatus() == Process::FINISHED) {
            hasFinished = true;
            cout << format("{:>8}   {}   Finished   {} / {}\n",
                pair.first, pair.second->getRunTimestamp(),
                pair.second->getCommandIndex()+1, pair.second->getTotalCommands());
        }
    }
    if (!hasFinished) {
        cout << "   No finished processes." << endl;
    }
    cout << "---------------------------------------" << endl << endl;
}

void ConsoleManager::schedulerTest() {
    std::thread([this]{
        schedulerTestRun = true;
        
        int cpuCycles = 1;
		    int i = 1;

        while (schedulerTestRun) {
            if (cpuCycles % configManager->getBatchProcessFreq() == 0) {
                string processName = "process" + to_string(i);
                createProcess(processName);
                i++;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            cpuCycles++;
        }
     }).detach();
    
}

void ConsoleManager::reportUtil() {
    // Write to a text file csopesy-log.txt
    string fileName = "csopesy-log.txt";  // Generate file name based on process name
    ofstream outFile(fileName, ios::out | ios::trunc);  // Open the file for writing and trunc if not empty

    if (outFile.is_open()){
        if (processes.empty()) {
            outFile << "> No processes available." << endl;
            return;
        }

        bool hasRunning = false, hasFinished = false;

		// TODO: Print out cpu utilization to text file. scheduler->displayCpuUtilization();
		Scheduler::CpuUtilization util = scheduler->getCpuUtilization();

        outFile << "CPU utilization: " << util.utilization << "%" << endl;
        outFile << "Cores used: " << util.usedCores << endl;
        outFile << "Cores available: " << util.availableCores << endl << endl;

        // Display queued processes
        outFile << "---------------------------------------" << endl;
        outFile << "Running processes:" << endl;
        for (const auto& pair : processes) {
            if (pair.second->getStatus() == Process::RUNNING) {
                hasRunning = true;
                outFile << format("{:>8}   {}   Core: {}   {} / {}\n",
                    pair.first, pair.second->getRunTimestamp(), pair.second->getCoreIndex(),
                    pair.second->getCommandIndex() + 1, pair.second->getTotalCommands());
            }
        }
        if (!hasRunning) {
            outFile << "   No running processes." << endl;
        }

        // Display finished processes
        outFile << endl;
        outFile << "Finished processes:" << endl;
        for (const auto& pair : processes) {
            if (pair.second->getStatus() == Process::FINISHED) {
                hasFinished = true;
                outFile << format("{:>8}   {}   Finished   {} / {}\n",
                    pair.first, pair.second->getRunTimestamp(),
                    pair.second->getCommandIndex() + 1, pair.second->getTotalCommands());
            }
        }
        if (!hasFinished) {
            outFile << "   No finished processes." << endl;
        }
        outFile << "---------------------------------------" << endl << endl;
        outFile.close();
    }

    // Display path to file
	cout << "> Report generated at " << fileName << "!" << endl;
}

void ConsoleManager::initialize(){
	configManager = new ConfigManager();
  memoryAllocator = new MemoryAllocator(configManager);
	scheduler = new Scheduler(configManager, memoryAllocator);
	initialized = true;
}

bool ConsoleManager::handleCommand(const string& command) {
    // Handle 'exit' when the user is inside a session
    if (inSession) {
        if (command == "exit") {
            cout << "> Exiting session..." << endl;
            inSession = false;  // Mark as no longer in a session
            currentSessionName = "";
            system("cls");  // Clear the screen when returning to the main menu
            printHeader();  // Re-display the main menu header
            return true;
        }
        else if (command == "process-smi") {
           processes[currentSessionName]->processSMI();
        }
    }

    // If not in a session, process general commands
    if (!inSession) {
        if (!initialized) {
            if (command == "initialize") {
                initialize();  // Initialize the system and load data from the file
				cout << GREEN << "> Processor configuration initialized." << RESET << endl;
            }
            else if (command == "exit") {
                // Exit the emulator
                cout << "> Exiting emulator..." << endl;
                exit(0);
            }
            else {
                // Unrecognized command before initialization
                cout << RED << "> Error: System is not initialized. Please type 'initialize'." << RESET << endl;
            }
        }
        else {
            if (command == "") {
                // If the user presses Enter without typing anything
                return true;
            }
            else if (command == "initialize") {
				// Command to initialize processer configuration and scheduler
                cout << GREEN << "> Already initialized." << RESET << endl;
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
                    bool res = createProcess(processName);  // Create a session
                    if (res) {
                        system("cls");
                        inSession = true;  // Mark that the user is now in a session
                        currentSessionName = processName; 
                    }
                    else {
                        cout << YELLOW << "> Error: Process already exists." << RESET << endl;
                    }
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
                    if (reattachProcess(processName)) {
                        system("cls");
                        inSession = true;  // Mark that the user is now in a session
                        currentSessionName = processName;  // Set the active session name
                    }
                }
            }
            else if (command == "screen -ls") {
                // List all active sessions
                system("cls");
                
                printHeader();
                cout << "root:\\> screen -ls" << endl;
                listProcess();
            }
            else if (command == "scheduler-test") {
                // Handle scheduler-test command
                cout << GREEN << "> Creating dummy processes" << RESET << endl;
                schedulerTest();
            }
            else if (command == "scheduler-stop") {
                // Handle scheduler-stop command
                cout << GREEN << "> Stopping creation of dummy processes" << RESET << endl;
                schedulerTestStop();
            }
            else if (command == "report-util") {
                // Handle report-util command
                reportUtil();
            }
            else if (command == "clear") {
                // Clear the screen and print the header again
                system("cls");
                printHeader();
            }
            else if (command == "help") {
                // Display the list of available commands
                cout << LIGHT_YELLOW << "> List of commands:" << endl
                    << "    - initialize            (initializes processor configuration and scheduler based on config.txt)" << endl
                    << "    - screen -s <name>      (start a new process)" << endl
                    << "    - screen -r <name>      (reattaches to an existing process)" << endl
                    << "    - screen -ls            (list all processes)" << endl
                    << "    - process-smi           (prints process info, only applicable when attached to a process)" << endl
                    << "    - scheduler-test        (starts the creation of dummy processes at configured intervals)" << endl
                    << "    - scheduler-stop        (stops the creation of dummy processes initiated by scheduler-test)" << endl
                    << "    - report-util           (generates a CPU utilization report and writes it to csopesy-log.txt)" << endl
                    << "    - clear                 (clears the screen)" << endl
					<< "    - help                  (displays list of commands)" << endl
                    << "    - exit                  (exits the emulator)" << RESET << endl;
            }
            else if (command == "exit") {
                // Exit the emulator
                cout << "> Exiting emulator..." << endl;
                exit(0);
            }
            else if (command == "process-smi") {
              system("cls");
              printHeader();
              cout << "root:\\> process-smi" << endl;
              memoryAllocator->showProcessSMI(scheduler->getCpuUtilization().utilization);
            }
            else if (command == "vmstat") {
              system("cls");
              printHeader();
              cout << "root:\\> vmstat" << endl;
							memoryAllocator->showVmStat(scheduler->idleCpuTicks, scheduler->activeCpuTicks);
						}
            else {
                // Handle unrecognized command
                cout << "> Unrecognized command: " << command << endl;
            }
        }
    }
    else {
        // If the user is in a session and tries an invalid command
        cout << "> You are in a process session. Use 'exit' to leave." << endl;
    }

    return true;
}

void ConsoleManager::printHeader() {
    cout << GREEN << "   _   _  _ ___ __  __ " << WHITE << " ___  ___  " << endl
        << GREEN << "  /_\\ | \\| |_ _|  \\/  |" << WHITE << "/ _ \\/ __| " << endl
        << GREEN << " / _ \\| .` || || |\\/| |" << WHITE << " (_) \\__ \\ " << endl
        << GREEN << "/_/ \\_\\_|\\_|___|_|  |_|" << WHITE << "\\___/|___/ " << endl
        << GREEN << "\nWelcome to ANIMOS Command Line Emulator!" << endl
        << LIGHT_YELLOW << "\nType 'help' to view all commands\n" << RESET << endl;
}