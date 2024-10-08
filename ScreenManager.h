#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <map>
#include <string>
#include <mutex>
#include "Process.h"

/**
 * @class ScreenManager
 * Manages multiple ScreenSession instances, allowing process creation, reattachment, and listing.
 *
 * The ScreenManager class handles the creation, management, and interaction with multiple processes.
 * It maintains a map of active processes, allowing users to create new processes, reattach to existing ones,
 * and list all active processes.
 */
class ScreenManager {
private:
    /**
     * A map of session names to their corresponding ScreenSession pointers.
     *
     * This map stores all active processes using the process name as the key.
     */
    std::map<std::string, Process*> processes;

    /**
     * A mutex to ensure thread safety when accessing/modifying processes.
     */
    std::mutex processMutex;

public:
    /**
     * Destructor for ScreenManager.
     *
     * This destructor ensures that all dynamically allocated Process instances are deleted
     * when the ScreenManager instance is destroyed.
     */
    ~ScreenManager();

    /**
     * Creates a new process if one does not already exist.
     *
     * If a process with the provided name does not already exist, this method creates a new Process instance.
     * It defaults the process to having 150 total lines and runs the process in a separate thread.
     * If the process already exists, it displays a message and ports the user into the existing process.
     *
     * @param name The name of the process to create.
     */
    void createProcess(const std::string& name);

    /**
     * Reattaches the user to an existing process.
     *
     * If a process with the provided name exists, this method reattaches the user to that process,
     * clears the screen, and redisplays the process details. If no process exists with that name,
     * it returns false.
     *
     * @param name The name of the process to reattach to.
     * @return true if the process was successfully reattached, false otherwise.
     */
    bool reattachProcess(const std::string& name);

    /**
     * Lists all active processes.
     *
     * This method displays a list of all currently active processes, showing the process name,
     * process ID, current instruction line, total lines, and the timestamp of when the process was created.
     * If no processes are active, it displays a message indicating that.
     */
    void listProcess();
};

#endif // SCREENMANAGER_H