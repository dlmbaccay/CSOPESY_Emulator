#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>

using namespace std;

class Process {
public:
    // Constructor
    Process(string name, int minCommands, int maxCommands, int minSize, int maxSize, int memPerFrame);
    ~Process() {}

    // Enum for process status
    enum Status { WAITING, READY, RUNNING, FINISHED };
    
    // Public methods
    void displayDetails() const;
    void processSMI();
    void getNextCommand();
    void execute();
    void setTimestamp();
    void setCoreIndex(int core) { coreIndex = core; }
    void setStatus(Status newStatus) { status = newStatus; }

	// Getters
	string getProcessName() const { return processName; }
	int getProcessId() const { return processId; }
	Status getStatus() const { return status; }
	bool getIsActive() const { return isActive; }
	size_t getTotalCommands() const { return commands.size(); }
    int getCommandIndex() const { return commandIndex; }
	int getCoreIndex() const { return coreIndex; }
	string getCreationTimestamp() const { return creationTimestamp; }
	string getRunTimestamp() const { return runTimestamp; }
	int getMemorySize() const { return memorySize; }
	int getNumPages() const { return numPages; }

	


private:
    // Private member variables
    Status status;            // Process status
    string processName;        // Name of the process
    int processId;             // Unique ID for the process
    vector<string> commands;   // List of commands for the process
    int commandIndex;          // Current command being executed
	string creationTimestamp;  // Timestamp when the process was created
    string runTimestamp;       // Timestamp when the process starts running
    bool isActive;             // Indicates if the process is active
    int coreIndex;             // The core index the process is assigned to
    int memorySize; 
    int numPages;
};

#endif // PROCESS_H
