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
    // this is the besed port and host
    // TODO: call = address_resolution(it->second.host);
    unsigned short int  port;
    unsigned int        host;

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
