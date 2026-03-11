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
