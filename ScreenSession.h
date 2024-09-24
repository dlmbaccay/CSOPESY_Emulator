#ifndef SCREENSESSION_H
#define SCREENSESSION_H

#include <string>

class ScreenSession {
public:
    std::string processName;
    int sessionId;
    int currentLine;
    int totalLines;
    std::string creationTimestamp;
    bool isActive;

    ScreenSession(std::string name, int totalLines);
    void displayDetails() const;
    void run();
    void stop();
};

#endif // SCREENSESSION_H