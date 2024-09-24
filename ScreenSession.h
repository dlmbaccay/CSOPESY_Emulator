#ifndef SCREENSESSION_H
#define SCREENSESSION_H

#include <string>

/**
 * @class ScreenSession
 * Represents an individual screen session for a process.
 *
 * The ScreenSession class simulates a process running in the background,
 * tracking its progress in terms of instruction lines. Each session has a unique
 * session ID, a process name, and other attributes such as total lines and timestamps.
 */
class ScreenSession {
public:
    /**
     * The name of the process associated with this session.
     */
    std::string processName;

    /**
     * A unique session ID, incremented for each session created.
     */
    int sessionId;

    /**
     * The current line of execution within the process.
     */
    int currentLine;

    /**
     * The total number of lines the session will execute.
     */
    int totalLines;

    /**
     * The timestamp when the session was created (formatted as MM/DD/YYYY, HH:MM:SS AM/PM).
     */
    std::string creationTimestamp;

    /**
     * Indicates whether the session is currently active or has been stopped.
     */
    bool isActive;

    /**
     * Constructs a new ScreenSession instance.
     *
     * Initializes a new session with a given name and the total number of lines. The session
     * is assigned a unique ID and sets its creation timestamp.
     *
     * @param name The name of the process to be associated with the session.
     * @param totalLines The total number of lines (steps) the session will execute.
     */
    ScreenSession(std::string name, int totalLines);

    /**
     * Displays the details of the session.
     *
     * This method outputs the process name, session ID, current instruction line, total lines,
     * and the timestamp when the session was created.
     */
    void displayDetails() const;

    /**
     * Simulates the running of the session.
     *
     * This method runs the session, incrementing the current instruction line every second.
     * The session continues until it reaches the total lines or is stopped.
     */
    void run();
};

#endif // SCREENSESSION_H