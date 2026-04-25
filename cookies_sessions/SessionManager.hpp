#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#define TIMEOUT_CS 1800 // 30 min

#include "Session.hpp"
#include "cookies_session_handler.hpp"
#include <iostream>
#include <algorithm>

class SessionManager {
    private:
        std::map<std::string, Session> sessions;

    public:
        Session& getSession(const std::string& sessionId);
        Session& createSession();
        std::string generateSessionId();
        SessionManager();
        SessionManager(const SessionManager& obj);
        SessionManager& operator=(const SessionManager& obj);
        ~SessionManager();
        bool sessionExists(const std::string& id);
        void sessionTimeCheck(std::string& id);
};

#endif