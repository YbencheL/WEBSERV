#include "ConfigPars.hpp"

int count_to_symbol(std::deque<Token>& tokenContainer, size_t& index, int count)
{
    index++;
    while(tokenContainer[index].value != ";")
    {
        count++;
        index++;
    }
    index--;
    return count;
}

void error_line(std::string msg, int Line)
{
    std::string errorLine;
    std::stringstream ss;

    ss << Line;
    errorLine = "ERROR on line " + ss.str() + msg;
    throw std::runtime_error(errorLine);
}

void duplicate_check(std::deque<std::string>& keywords, std::string name)
{
    int count = 0;

    for (size_t i = 0; i < keywords.size(); i++)
    {
        if (keywords[i] == name)
            count++;
        else if (keywords[i] == "server")
            count = 0;
        if (count > 1)
            throw std::runtime_error("ERROR: there must be no duplicates for these keywords: listen, client_max_body_size and server_name, or a duplicated method inside allow_methods directive");
    }
}

void checking_for_keyword_dups(std::deque<Token>& tokenContainer)
{
    std::deque<std::string> keywords;

    for (size_t i = 0; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 0)
            keywords.push_back(tokenContainer[i].value);
    }
    duplicate_check(keywords, "listen");
    duplicate_check(keywords, "client_max_body_size");
    duplicate_check(keywords, "server_name");
}

void checking_for_defaults(ServerBlock& Serv)
{
    if ((Serv.listen < PORT_MIN_VAL || Serv.listen > PORT_MAX_VAL))
        throw std::runtime_error("ERROR: port has incorrect value must be between 1024 and 65535");
    else if (Serv.client_max_body_size < 0)
        throw std::runtime_error("ERROR: client_max_body_size has incorrect value");
    else if (!Serv.listen)
        throw std::runtime_error("ERROR: missing value (listen)");
    if (Serv.error_page.empty()) Serv.error_page.insert(std::make_pair(404, "/default_error_path"));
    if (Serv.host.empty()) Serv.host = "127.0.0.1";
    if (Serv.server_name.empty()) Serv.server_name = "WEBSERV_42";
    if (Serv.index.empty()) Serv.index.push_back("index.html");
    if (!Serv.client_max_body_size) Serv.client_max_body_size = CLIENT_MAX_BODY_SIZE;
    for (size_t i = 0; i < Serv.locations.size(); i++)
    {
        if (Serv.locations[i].root.empty())
        {
            Serv.locations[i].root = Serv.root;
            if (Serv.locations[i].root.empty())
                throw std::runtime_error("ERROR: missing value (root)");

        }
        if (Serv.locations[i].index.empty()) Serv.locations[i].index = Serv.index;
        if (Serv.locations[i].allow_methods.empty())
        {
            Serv.locations[i].allow_methods.push_back("GET");
            Serv.locations[i].allow_methods.push_back("POST");
            Serv.locations[i].allow_methods.push_back("DELETE");
        }
    }
}

void checking_for_virtual_hosts(std::multimap<int, std::string>& seen, std::string& msg)
{
    size_t key = 0;
    size_t count = 0;
    std::string value;

    for (std::map<int, std::string>::iterator it = seen.begin();
        it != seen.end(); ++it)
    {
        count = 0;
        key = it->first;
        value = it->second;
        std::multimap<int, std::string>::iterator lower = seen.lower_bound(key);
        std::multimap<int, std::string>::iterator upper = seen.upper_bound(key);
        for (std::map<int, std::string>::iterator it = lower;
            it != upper; ++it)
        {
            if (value == it->second)
                count++;
        }
        if (count > 1)
            throw std::runtime_error(msg);
    }
}