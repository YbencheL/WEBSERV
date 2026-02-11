#include "ConfigPars.hpp"

int count_to_symbol(std::deque<Token>& tokenContainer, ssize_t& index, int count)
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
            throw std::runtime_error("ERROR: there must be no duplicates for keywords");
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
    // duplicate_check(keywords, "client_max_body_size");
    duplicate_check(keywords, "server_name");
    duplicate_check(keywords, "host");
}

void checking_values(ServerBlock& Serv)
{
    std::deque<std::string> seenLocationPaths;

    if (!Serv.listen)
        throw std::runtime_error("ERROR: missing port value");
    else if (Serv.listen < PORT_MIN_VAL || Serv.listen > PORT_MAX_VAL)
        throw std::runtime_error("ERROR: port has incorrect value must be between 1024 and 65535");
    else if (Serv.client_max_body_size < 0 || !Serv.client_max_body_size)
        throw std::runtime_error("ERROR: client_max_body_size has incorrect value");
    else if (Serv.error_page.empty())
        throw std::runtime_error("ERROR: error_page has incorrect value");
    else if (Serv.host.empty())
        throw std::runtime_error("ERROR: host has incorrect value");
    // else if (Serv.server_name.empty())
    //     throw std::runtime_error("ERROR: server_name has incorrect value");
    else if (Serv.index.empty())
        throw std::runtime_error("ERROR: index has incorrect value");
    // these values will have a default if they dont exist
    for (size_t i = 0; i < Serv.locations.size(); i++)
    {
        seenLocationPaths.push_back(Serv.locations[i].path);
        duplicate_check(seenLocationPaths, Serv.locations[i].path);
        // inheritance logic
        if (Serv.locations[i].root.empty())
        {
            Serv.locations[i].root = Serv.root;
            if (Serv.locations[i].root.empty())
                throw std::runtime_error("ERROR: missing value (root)");
        }
        if (!Serv.locations[i].client_max_body_size)
        {
            Serv.locations[i].client_max_body_size = Serv.client_max_body_size;
            if (!Serv.locations[i].client_max_body_size)
                throw std::runtime_error("ERROR: missing value (client_max_body_size)");
        }
        if (Serv.locations[i].index.empty())
        {
            Serv.locations[i].index = Serv.index;
            if (Serv.locations[i].index.empty())
                throw std::runtime_error("ERROR: missing value (index)");
        }
        if (Serv.locations[i].error_page.empty())
        {
            Serv.locations[i].error_page = Serv.error_page;
            if (Serv.locations[i].error_page.empty())
                throw std::runtime_error("ERROR: missing value (index)");
        }
        // if (Serv.locations[i].index.empty()) Serv.locations[i].index = Serv.index;
        // if allow method directive is empty its gonna have these three
        if (Serv.locations[i].allow_methods.empty())
        {
            Serv.locations[i].allow_methods.push_back("GET");
            Serv.locations[i].allow_methods.push_back("POST");
            Serv.locations[i].allow_methods.push_back("DELETE");
        }
    }
}

void checking_for_virtual_hosts(std::deque<int>& seen)
{
    ssize_t count = 0;
    int value = 0;

    // checking if there is the same ip in multiple server block
    for (std::deque<int>::iterator it = seen.begin(); it != seen.end(); ++it)
    {
        value = *it;
        count = std::count(seen.begin(), seen.end(), value);
        if (count > 1)
            throw std::runtime_error("ERROR: there must be only unique ports per server block");
    }
}

// used to check only for two server with both same port and ip or server name
// void checking_for_virtual_hosts(std::multimap<int, std::string>& seen, std::string& msg)
// {
//     size_t key = 0;
//     size_t count = 0;
//     std::string value;

//     for (std::map<int, std::string>::iterator it = seen.begin();
//         it != seen.end(); ++it)
//     {
//         count = 0;
//         key = it->first;
//         value = it->second;
//         std::multimap<int, std::string>::iterator lower = seen.lower_bound(key);
//         std::multimap<int, std::string>::iterator upper = seen.upper_bound(key);
//         for (std::map<int, std::string>::iterator it = lower;
//             it != upper; ++it)
//         {
//             if (value == it->second)
//                 count++;
//         }
//         if (count > 1)
//             throw std::runtime_error(msg);
//     }
// }