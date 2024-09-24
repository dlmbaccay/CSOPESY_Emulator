#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <map>
#include <string>
#include <mutex>
#include "ScreenSession.h"

class ScreenManager {
private:
    std::map<std::string, ScreenSession*> sessions;
    std::mutex sessionMutex;

public:
    ~ScreenManager();
    void createSession(const std::string& name);
    bool reattachSession(const std::string& name);
    void listSessions();
};

#endif // SCREENMANAGER_H