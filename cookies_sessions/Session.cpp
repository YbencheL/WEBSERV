#include "Session.hpp"

Session::Session() : id(""), last_access(0) {}

Session& Session::operator=(const Session& obj)
{
    if (this != &obj)
    {
        id = obj.id;
        data = obj.data;
        last_access = obj.last_access;
        is_new = obj.is_new;
    }
    return *this;
}

Session::Session(const Session& obj)
{
    id = obj.id;
    data = obj.data;
    last_access = obj.last_access;
    is_new = obj.is_new;
}

std::map<std::string, std::string>& Session::GetData()
{
    return data;
}

std::string& Session::GetSessionVal(std::string key)
{
    return data[key];
}

void Session::SetSessionVal(std::string key, std::string value)
{
    data[key] = value;
}

Session::~Session() {}