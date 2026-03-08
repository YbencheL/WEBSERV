# include "../response_builder.hpp"
# include "../utils/utils.hpp"
# include "../client.hpp"
# include <stack>

response_builder::response_builder(): is_body_ready(false), is_error_page(false) {};

void    response_builder::init_response_builder(Client &current_client) {
    this->current_client = &current_client;
}

// bool    response_builder::is_allowd_method(std::string method)
// {
//     for (size_t i = 0; i < current_client->location_conf->allow_methods.size(); i++) {
//         if (current_client->location_conf->allow_methods.at(i) == method)
//             return (true);
//     }
//     return (false);
// }

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

// TODO-LATER: Methode not allowed
void response_builder::build_response()
{
    std::cout << READ_S << "--------- Methode: " << current_client->req.getMethod() << READ_E << std::endl;
    std::cout << READ_S << "--------- Path: " << current_client->req.getPath() << READ_E << std::endl;

    path_validation();  // TOKNOW: auto-index gen
    
    std::cout << "STATUS CODE " << current_client->res.get_stat_code() << std::endl;

    if (this->current_client->res.get_stat_code() != OK)
        generate_error_page();  // DONE [-]

    else if (this->current_client->req.getMethod() == GET_METHODE)
        handle_get();   // DONE [-] working on it

    else if (this->current_client->req.getMethod() == POST_METHODE)
        handle_post();  // DONE [-] working on it
    
    else if (this->current_client->req.getMethod() == DELETE_METHODE)
        handle_delete();    // DONE [+]

    this->current_client->res.set_raw_response(response_holder);
    
    std::cout << GREEN_S << "--------- START RESPONSE\n" << response_holder << "\n------- END RESPONSE" << GREEN_E << std::endl;
}
