#include "../response_builder.hpp"
#include "../socket_engine.hpp"
#include "../utils/utils.hpp"

// COOKIE NOTE
// if (this->is_cooke_set) {
//     for (size_t i = 0; i < cookie_holder.size(); ++i) {
//         raw_response += "Set-Cookie: " + cookie_holder[i] + "\r\n";
//     }
// }

// TODO CREATE A funciont that append the cookies to the header

void response_builder::set_header(void)
{
    response_holder.append(current_client->res.get_start_line());
    response_holder.append("Server: Webserv\r\n");
    response_holder.append("Date: " + get_time() + "\r\n");

    if (is_body_ready || is_error_page)
        response_holder.append("Content-Type: text/html\r\n");
    else
        response_holder.append("Content-Type: " + extension_to_media_type(this->path) + "\r\n");
    
    if (this->current_client->res.get_is_cookie_set())
    {
        // TODO: create a func to done that
        // std::cout << "yes it's a fucking cookies here to done #1" << std::endl;
        // exit(1);
    }
}

void response_builder::set_body(void)
{
    if (!is_body_ready)
        serving_static_file();
    else {
        response_holder.append("Content-Length: " + to_string(this->body_buff.size()) + "\r\n\r\n");
        response_holder.append(this->body_buff);
    }
}

void response_builder::generate_error_page()
{
    this->is_error_page = true;
    unsigned short int status_code = this->current_client->res.get_stat_code();

    if (this->current_client->server_conf)
        this->path = get_stat_code_path(status_code);

    // std::cout << "DELETE PATH -> " << this->path << std::endl;
    if (is_valid_error_path(this->path))
        serving_static_file();
    else{
        set_header();
        default_error_page(status_code);
    }
}

void response_builder::handle_get()
{
    set_header();
    set_body();
}
