#ifndef CLIENT_HPP
#define CLIENT_HPP

# include <iostream>
# include "request/includes/request.hpp"
# include "request/includes/parseRequest.hpp"
# include "response.hpp"

class Request;
struct reqParse;


// MAIN
// will add every thing need between [req/res] 
struct Client
{
    // infos needed
    int         port;
    std::string host;
    /// get the server-level match
    const ServerBlock *server_conf;
    /// get the location-level match
    const LocationBlock *location_conf;

    Request req;
	reqParse parse;
    response res;
    bool    reqReady;

    // about the timeout check
    unsigned int last_activity;
};

void inisializeClient(Client &client);


#endif