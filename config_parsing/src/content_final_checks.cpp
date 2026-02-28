#include "../includes/ConfigPars.hpp"

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

    if (Line != -1)
    {
        ss << Line;
        errorLine = "ERROR on line " + ss.str() + msg;
        throw std::runtime_error(errorLine);
    }else
        throw std::runtime_error("ERROR" + msg);
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
    std::vector<std::string> non_duplicated_keyword;

    for (size_t i = 0; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 0)
            keywords.push_back(tokenContainer[i].value);
    }
    non_duplicated_keyword.push_back("listen");
    non_duplicated_keyword.push_back("server_name");
    non_duplicated_keyword.push_back("host");
    for(size_t i = 0; i < non_duplicated_keyword.size(); i++)
        duplicate_check(keywords, non_duplicated_keyword[i]);
}

template <typename T>
void inherit_check(T& member, T& defaultValue, const std::string& fieldName)
{
    if (member.empty())
    {
        member = defaultValue;
        if (member.empty())
            throw std::runtime_error("ERROR: missing value " + fieldName);
    }
}

void empty_values_check(ServerBlock&  Serv)
{
    if (Serv.error_page.empty() || Serv.host.empty() || Serv.index.empty())
        throw std::runtime_error("ERROR: missing value (error_page, host or index)");
}


void checking_values(ServerBlock& Serv)
{
    std::deque<std::string> seenLocationPaths;

    if (!Serv.listen)
        throw std::runtime_error("ERROR: missing value (port)");
    else if (!Serv.set_timeout)
        Serv.set_timeout = 100;
    else if (Serv.listen < PORT_MIN_VAL || Serv.listen > PORT_MAX_VAL)
        throw std::runtime_error("ERROR: port has incorrect value must be between 1024 and 65535");
    else if (Serv.client_max_body_size < 0 || !Serv.client_max_body_size)
        throw std::runtime_error("ERROR: client_max_body_size has incorrect value");
    //checking empty values
    empty_values_check(Serv);
    // these values will have a default if they dont exist
    for (size_t i = 0; i < Serv.locations.size(); i++)
    {
        seenLocationPaths.push_back(Serv.locations[i].path);
        duplicate_check(seenLocationPaths, Serv.locations[i].path);
        // inheritance logic
        if (!Serv.locations[i].client_max_body_size)
        {
            Serv.locations[i].client_max_body_size = Serv.client_max_body_size;
            if (!Serv.locations[i].client_max_body_size)
                throw std::runtime_error("ERROR: missing value (client_max_body_size)");
        }
        inherit_check(Serv.locations[i].root, Serv.root, "root");
        inherit_check(Serv.locations[i].index, Serv.index, "index");
        inherit_check(Serv.locations[i].error_page, Serv.error_page, "error_page");
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