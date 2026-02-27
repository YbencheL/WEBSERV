# include "../response_builder.hpp"
# include "../socket_engine.hpp"    // used just to indelucde the client struct //  rm-me
# include "../utils/utils.hpp"
# include "../client.hpp"
# include <stack>


// -------------------- HARDCODED --------------------------
std::string _method_ = "GET";
// std::string _method_ = "POST";
// std::string _method_ = "DELETE";

std::string _protocol_ = "HTTP/1.0";

// static std::string _host_ = "10.11.11.4";
// static int _port_ = 8080;

static std::string _host_ = "localhost";
static int _port_ = 9090;

// -------------------------------------------------------

// current_client->server_conf;
// current_client->location_conf;

response_builder::response_builder(): is_body_ready(false), is_error_page(false) {};

bool    response_builder::is_allowd_method(std::string method)
{
    for (size_t i = 0; i < current_client->location_conf->allow_methods.size(); i++) {
        if (current_client->location_conf->allow_methods.at(i) == method)
            return (true);
    }
    return (false);
}

std::string response_builder::index_file_iterator(const std::string &full_path)
{
    std::string redirection_path;
    std::string based_path = full_path;
    if (!full_path.empty() && full_path.at(full_path.length() -1) != '/')
        based_path += '/';

    for (size_t i = 0; i < current_client->location_conf->index.size(); i++) {
        redirection_path = based_path + current_client->location_conf->index.at(i);
        if (access(redirection_path.c_str(), F_OK | R_OK) == 0)
            return (redirection_path);
    }
    return ("");
}

void response_builder::response_setup()
{
    std::cout << "STATUS CODE " << current_client->res.get_stat_code() << std::endl;
    if (current_client->res.get_stat_code() != OK)  // error page
        generate_error_page();
    else if (_method_ == GET_METHODE)
        handle_get();
    else if (_method_ == POST_METHODE)
        handle_post();
    else if (_method_ == DELETE_METHODE)
        handle_delete();
    this->current_client->res.set_raw_response(response_holder);
}

// void    response_builder::init_response_builder(Client &current_client, std::deque<ServerBlock> &config) {
void    response_builder::init_response_builder(Client &current_client) {
    this->current_client = &current_client;
}

void    response_builder::validate_headers(std::map<std::string, std::string> header)
{
    // here will parse the header infos needed //
    /*
        extract from the request header:
            Host:
                based on host will extract:
                    server-level block
                    location-level block
                        based on both:
                            will get the infos looking for
                                allowed methodes
                                max-client-body-size
                                CGI
    */

}


void response_builder::build_response()
{
    if (this->current_client->res.get_stat_code() == OK) {
        if (!is_allowd_method(_method_))
            this->current_client->res.set_stat_code(METHOD_NOT_ALLOWED);
        else    // >>> join the root with path
            path_validation();
    }
    response_setup();
}
