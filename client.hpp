#ifndef CLIENT_HPP
#define CLIENT_HPP

# include <iostream>
# include "request/includes/request.hpp"
# include "request/includes/parseRequest.hpp"
# include "response.hpp"

# define GREEN_S "\033[0;32m"
# define GREEN_E "\033[0m"

# define READ_S "\033[31m"
# define READ_E "\033[0m"

// default extansion in case (No name, No content-type)
# define DEFAULT_EXTENSION ".txt"
// how the browser threat the data
# define DEFAULT_MEDIA_TYPE "text/plain"

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
    std::string         host_str_format;

    /// get the location-level match

    // ServerBlockLookup
    ServerBlockLookup       config_file_info;
    const ServerBlock       *server_conf;
    const LocationBlock     *location_conf;

    Request req;
	reqParse parse;
    response res;
    bool    reqReady;

    // about the timeout check
    unsigned int    last_activity;
    bool            close_connection;

    /*  TODO saving
        open fd
        content_length
        written_bytes
    */
};

void inisializeClient(Client &client);


#endif
