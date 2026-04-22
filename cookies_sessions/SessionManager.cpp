#include "SessionManager.hpp"

SessionManager::SessionManager() {}

SessionManager& SessionManager::operator=(const SessionManager& obj)
{
    if (this != &obj)
        this->sessions = obj.sessions;
    return *this;
}

SessionManager::SessionManager(const SessionManager& obj)
{
    this->sessions = obj.sessions;
}

SessionManager::~SessionManager() {}

Session& SessionManager::getSession( const std::string& sessionId)
{

    if (sessions.find(sessionId) != sessions.end())
        return sessions[sessionId];
    else
        return createSession();
}

std::string SessionManager::generateSessionId()
{
    const std::string charset =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    std::string key;
    int i = 0;
    ssize_t sizeOfArr = charset.size() - 1;
    
    while(i < 16)
    {
        key.push_back(charset[rand() % sizeOfArr]);
        i++;
    }
    return key;
}

Session& SessionManager::createSession()
{
    std::string sessionID;
    Session Ses;

    // generating id
    sessionID = generateSessionId();
    // storing session obj in sessions map
    Ses.last_access = std::time(0);
    Ses.id = sessionID;
    Ses.is_new = true;
    sessions[sessionID] = Ses;
    return sessions[sessionID];
}

bool SessionManager::sessionExists(const std::string& id)
{
    if (sessions.find(id) != sessions.end())
        return true;
    else
        return false;
}

void SessionManager::sessionTimeCheck(std::string& id)
{
    time_t current_time;

    current_time = std::time(0);
    if (current_time - sessions[id].last_access > TIMEOUT_CS)
        sessions.erase(id);
}