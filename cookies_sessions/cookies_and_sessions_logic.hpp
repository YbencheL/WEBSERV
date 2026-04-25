#ifndef COOKIES_AND_SESSIONS_LOGIC_HPP
#define COOKIES_AND_SESSIONS_LOGIC_HPP

#include "cookies_session_handler.hpp"
#include "../client.hpp"
#include "SessionManager.hpp"
#include "Session.hpp"   

Session& cookies_and_sessions_logic(SessionManager& sessionM, Client& client);
#endif