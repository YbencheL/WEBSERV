# include "../response_builder.hpp"
// # include "../socket_engine.hpp"
# include "../utils/utils.hpp"
# include "../client.hpp"
# include <stack>

response_builder::response_builder(): is_body_ready(false), is_error_page(false) {};

void    response_builder::init_response_builder(Client &current_client) {
    this->current_client = &current_client;
}

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
    {
        generate_error_page();
    }
    else if (current_client->req.getMethod() == GET_METHODE)
        handle_get();
    else if (current_client->req.getMethod() == POST_METHODE)
        handle_post();
    else if (current_client->req.getMethod() == DELETE_METHODE)
        handle_delete();
    this->current_client->res.set_raw_response(response_holder);
    
    std::cout << "--------- START RESPONSE\n" << response_holder << "\n------- END RESPONSE" << std::endl;

}

void response_builder::build_response()
{
    if (this->current_client->res.get_stat_code() == OK) {
        if (!is_allowd_method(current_client->req.getMethod()))
            this->current_client->res.set_stat_code(METHOD_NOT_ALLOWED);
        else
            path_validation();
    }
    response_setup();
}
