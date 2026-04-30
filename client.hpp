#ifndef CLIENT_HPP
#define CLIENT_HPP

# include <iostream>
# include "request/includes/request.hpp"
# include "request/includes/parseRequest.hpp"
# include "response.hpp"
#include "cgi/cgi.hpp"

# define GREEN "\033[0;32m"
# define RED "\033[31m"
# define BLUE "\x1B[36m"
# define YELLOW "\x1B[93m"
# define PINK "\x1B[35m"
# define RSET "\033[0m"

# define DEFAULT_EXTENSION ".txt"
# define DEFAULT_MEDIA_TYPE "text/plain"

class Request;
struct reqParse;

struct Client
{
    unsigned short int  port;
    unsigned int        host;
    std::string         host_str_format;

    ServerBlockLookup       config_file_info;
    const ServerBlock       *server_conf;
    const LocationBlock     *location_conf;

    Request     req;
	reqParse    parse;
    response    res;
    bool        reqReady;
    Cgi         cgiHandler;

    bool            is_serving_file;
    int             static_file_fd;
    off_t           file_size;
    off_t           bytes_sent;
    unsigned int    last_activity;
    bool            close_connection;
};

void inisializeClient(Client &client);

#endif
