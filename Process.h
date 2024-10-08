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
    /**
     * The name of the process.
     */
    std::string processName;

    /**
     * A unique process ID, incremented for each process created.
     */
    int processId;

    /**
     * The current line of execution within the process.
     */
    int currentLine;

    /**
     * The total number of lines the process will execute.
     */
    int totalLines;

    /**
     * The timestamp when the process was created (formatted as MM/DD/YYYY, HH:MM:SS AM/PM).
     */
    std::string creationTimestamp;

    /**
     * Indicates whether the process is currently active or has been stopped.
     */
    bool isActive;

    /**
     * Constructs a new Process instance.
     *
     * Initializes a new process with a given name and the total number of lines. The process
     * is assigned a unique ID and sets its creation timestamp.
     *
     * @param name The name of the process.
     * @param totalLines The total number of lines (steps) the process will execute.
     */
    Process(std::string name, int totalLines);

    /**
     * Displays the details of the process.
     *
     * This method outputs the process name, process ID, current instruction line, total lines,
     * and the timestamp when the process was created.
     */
    void displayDetails() const;

    /**
     * Simulates the running of the process.
     *
     * This method runs the process, incrementing the current instruction line every second.
     * The process continues until it reaches the total lines or is stopped.
     */
    void run();
};

#endif // PROCESS_H
