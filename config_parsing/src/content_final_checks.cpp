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
            error_line(": there must be no duplicates for keywords", -1);
    }
}

template <typename T>
void inherit_check(T& member, T& defaultValue, const std::string& fieldName)
{
    if (member.empty())
    {
        member = defaultValue;
        if (member.empty())
            error_line(": missing value " + fieldName, -1);
    }
}

void empty_values_check(ServerBlock&  Serv)
{
    if (Serv.error_page.empty() || Serv.host.empty() || Serv.index.empty())
        error_line(": missing value (error_page, host or index)", -1);
}


void checking_values(ServerBlock& Serv)
{
    std::deque<std::string> seenLocationPaths;

    if (!Serv.listen)
        error_line(": missing value (port)", -1);
    else if (!Serv.set_timeout)
        Serv.set_timeout = 100;
    else if (Serv.listen < PORT_MIN_VAL || Serv.listen > PORT_MAX_VAL)
        error_line(": port has incorrect value must be between 1024 and 65535", -1);
    else if (Serv.client_max_body_size < 0 || !Serv.client_max_body_size)
        error_line(": client_max_body_size has incorrect value", -1);
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
                error_line(": missing value (client_max_body_size)", -1);
        }
        inherit_check(Serv.locations[i].root, Serv.root, "root");
        inherit_check(Serv.locations[i].index, Serv.index, "index");
        inherit_check(Serv.locations[i].error_page, Serv.error_page, "error_page");
        // if allow method directive is empty its gonna have these three
        if (Serv.locations[i].allow_methods.empty())
        {
            Serv.locations[i].allow_methods.insert("GET");
            Serv.locations[i].allow_methods.insert("POST");
            Serv.locations[i].allow_methods.insert("DELETE");
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
            error_line(": there must be only unique ports per server block", -1);
    }
}