#ifndef PROCESS_H
#define PROCESS_H

#include <string>

/**
 * @class Process
 * Represents an individual process session.
 *
 * The Process class simulates a background process, tracking its progress
 * in terms of instruction lines. Each process has a unique process ID, a name,
 * and other attributes such as total lines and timestamps.
 */
class Process {
public:
    enum Status { Queued, Running, Finished };
    Status status;

    std::string processName;
    int processId;
    int currentLine;
    int totalLines;
    std::string runTimestamp;
    bool isActive;
    int coreIndex;

    Process(std::string name, int totalLines);

    void displayDetails() const;

    void run(int coreIndex);

    void setTimestamp();
};

#endif // PROCESS_H
