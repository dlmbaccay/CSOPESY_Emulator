#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <map>
#include <string>
#include <mutex>
#include "ScreenSession.h"

/**
 * @class ScreenManager
 * Manages multiple ScreenSession instances, allowing session creation, reattachment, and listing.
 *
 * The ScreenManager class handles the creation, management, and interaction with multiple screen sessions.
 * It maintains a map of active sessions, allowing users to create new sessions, reattach to existing ones,
 * and list all active sessions.
 */
class ScreenManager {
private:
    /**
     * A map of session names to their corresponding ScreenSession pointers.
     *
     * This map stores all active screen sessions using the session name as the key.
     */
    std::map<std::string, ScreenSession*> sessions;

    /**
     * A mutex to ensure thread safety when accessing/modifying sessions.
     */
    std::mutex sessionMutex;

public:
    /**
     * Destructor for ScreenManager.
     *
     * This destructor ensures that all dynamically allocated ScreenSession instances are deleted
     * when the ScreenManager instance is destroyed.
     */
    ~ScreenManager();

    /**
     * Creates a new screen session if one does not already exist.
     *
     * If a session with the provided name does not already exist, this method creates a new ScreenSession instance.
     * It defaults the session to having 150 total lines and runs the session in a separate thread.
     * If the session already exists, it displays a message and ports the user into the existing session.
     *
     * @param name The name of the session to create.
     */
    void createSession(const std::string& name);

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
    bool reattachSession(const std::string& name);

    /**
     * Lists all active screen sessions.
     *
     * This method displays a list of all currently active screen sessions, showing the session name,
     * session ID, current instruction line, total lines, and the timestamp of when the session was created.
     * If no sessions are active, it displays a message indicating that.
     */
    void listSessions();
};

#endif // SCREENMANAGER_H