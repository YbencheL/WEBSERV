# include "../response_builder.hpp"
# include "../utils/utils.hpp"
# include "../client.hpp"
# include <stack>

response_builder::response_builder(): is_body_ready(false), is_error_page(false) {};

void    response_builder::init_response_builder(Client &current_client) {
    this->current_client = &current_client;
}

std::string response_builder::index_file_iterator(const std::string &full_path)
{
    std::string redirection_path;
    std::string based_path = full_path;
    if (!full_path.empty() && full_path.at(full_path.length() -1) != '/')
        based_path += '/';

    // std::set<std::string>::iterator 
    for (size_t i = 0; i < current_client->location_conf->index.size(); i++)
    {
        // redirection_path = based_path + current_client->location_conf->index.;
        // redirection_path = based_path + current_client->location_conf->index.begin();
        if (access(redirection_path.c_str(), F_OK | R_OK) == 0)
            return (redirection_path);
    }
    return ("");
}

void    response_builder::serving_static_file(std::string path)
{
    (void)path;

    struct stat st;
    stat(this->path.c_str(), &st);

    int fd = open(this->path.c_str(), O_RDONLY);
    if (fd == -1) {
        this->current_client->res.set_stat_code(FORBIDDEN_ACCESS);
        this->set_header();
        this->default_error_page(this->current_client->res.get_stat_code());
        return ;
    }
    this->current_client->res.set_static_file_fd(fd);
    this->current_client->is_serving_file = true;

    std::cout << "[>] static file: " << this->path << " size: " << st.st_size << std::endl; // rm-me
    current_client->res.set_file_size(st.st_size);

    // ______________________________________header______________________________________
    this->header_buff.append(current_client->res.get_start_line());
    this->header_buff.append("Server: Webserv\r\n");
    this->header_buff.append("Date: " + get_time() + "\r\n");
    if (is_error_page)
        this->header_buff.append("Content-Type: text/html\r\n");
    else
        this->header_buff.append("Content-Type: " + extension_to_media_type(this->path) + "\r\n");
    this->header_buff.append("Content-Length: " + to_string(st.st_size) + "\r\n\r\n");
    // __________________________________________________________________________________

    this->response_holder = header_buff;
    // std::cout << ""
}

// TODO-LATER: Methode not allowed
void response_builder::build_response()
{
    // std::cout << READ_S << "--------- Methode: " << current_client->req.getMethod() << READ_E << std::endl;
    // std::cout << READ_S << "--------- Path: " << current_client->req.getPath() << READ_E << std::endl;
    // std::cout << "[>] STATUS CODE " << current_client->res.get_stat_code() << std::endl;
    // std::cout << "[>] getMethod " << current_client->req.getMethod() << std::endl;

    if (this->current_client->res.get_stat_code() != OK) {
        exit(2);
        generate_error_page();  // DONE [-] working on it
    }
    else
    {
        // exit(3);
        path_validation();  // TOKNOW: auto-index gen
        if (this->current_client->res.get_stat_code() != OK) {
            generate_error_page();  // DONE [-] working on it
            // exit(3);
            // this->is_body_ready = true;
            // set_header();
            // default_error_page(this->current_client->res.get_stat_code());
        } else {
            if (this->current_client->req.getMethod() == GET_METHODE)
            {
                // exit(1);
                handle_get();   // DONE [-] working on it
            }

            else if (this->current_client->req.getMethod() == POST_METHODE)
                handle_post();  // DONE [-] working on it
            
            else if (this->current_client->req.getMethod() == DELETE_METHODE)
                handle_delete();    // DONE [+]
        }

    }
    // std::cout << "[>] STATUS CODE " << current_client->res.get_stat_code() << std::endl;
    this->current_client->res.set_raw_response(response_holder);
    // std::cout << GREEN_S << "--------- START RESPONSE\n" << response_holder << "\n------- END RESPONSE" << GREEN_E << std::endl;
}
