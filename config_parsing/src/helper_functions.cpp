#include "../includes/ConfigPars.hpp"

in_addr_t address_resolution(std::string host)
{
    struct addrinfo hints, *result;
    in_addr_t host_re;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), NULL, &hints, &result) != 0)
        return (INADDR_NONE);

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
    
    host_re = ipv4->sin_addr.s_addr;

    freeaddrinfo(result);
    return (host_re);
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

bool    is_cgi_path_valid(std::string interpreter_path)
{
    if (access(interpreter_path.c_str(), F_OK | X_OK) < 0)
        return (false);
    return (true);
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