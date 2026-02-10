# include "../response.hpp"

// response::response()
    // std::cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxclaedxxxxxxxxxxxxxxxxxxxxx" << std::endl;
    // stat_code = 200;

    // str_stat_code[200] = "OK";
    // str_stat_code[403] = "Forbidden";
    // str_stat_code[404] = "Not Found";
    // str_stat_code[405] = "Method Not Allowed";
    // str_stat_code[500] = "Internal Server Error";

response::response() {};

response::response(const request& req)
{
    (void)req;
    this->stat_code = 200;
    this->content_length = -1;
    this->is_body_ready = false;
    this->resolved_path = "";

    // REQUEST HARDCODED VALUES TO TEST
    method = "GET";
    protocol = "HTTP/1.0";
    path = "/";
    host = "localhost";
    port = 8080;
}

void   response::response_handler(std::deque<ServerBlock> &server_config_info)
{
    // get server block match
    const ServerBlock *server_req = getServerForRequest(host, port, server_config_info);
    
    if (server_req != NULL) {
        const LocationBlock *getLocatoin = getLocation(path, *server_req);   // get location
        if (getLocatoin != NULL) // find location
        {
            // check the method is allowd
            if (is_allowd_method(getLocatoin->allow_methods, method)) {
                // check the path is exist
                path_validation(getLocatoin, *this);
            } else {
                set_stat_code(METHOD_NOT_ALLOWED);
            }
        } else    // location not found
            set_stat_code(NOT_FOUND);
    }
    else
        set_stat_code(NOT_FOUND);
}

void    response::set_stat_code(unsigned short int stat_code) {
    this->stat_code = stat_code;
}

void    response::set_body_contnet(std::string body) {
    this->body = body;
    content_length = body.length();
}

void    response::set_body_as_ready(void) {
    this->is_body_ready = true;
}

void    response::set_resolved_path(std::string &re_path) {
    this->resolved_path = re_path;
}

unsigned short int response::get_stat_code(void) {
    return (this->stat_code);
}

ssize_t   response::get_content_length(void) {
    return (this->content_length);
}

std::string response::get_path(void) {
    return (this->path);
}

bool    response::get_is_body_ready(void) {
    return (this->is_body_ready);
}

