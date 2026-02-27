# include "../response_builder.hpp"
# include "../socket_engine.hpp"    // use to use client struct
# include "../utils/utils.hpp"

void    response_builder::set_header(void)
{
    response_holder.append(current_client->res.get_start_line());
    response_holder.append("Server: Webserv\r\n");
    response_holder.append("Date: " + get_time() + "\r\n");
    
    if (is_body_ready || is_error_page)
        response_holder.append("Content-Type: text/html\r\n");  // just in case of autoindex //
    else
        response_holder.append("Content-Type: " + resolved_path_extension(this->path) + "\r\n");
}

void    response_builder::set_body(void)
{
    if (!is_body_ready)
        this->body = file_to_string(this->path);

    response_holder.append("Content-Length: " + to_string(this->body.size()) + "\r\n\r\n");
    response_holder.append(this->body);
}

void    response_builder::generate_error_page()
{
    unsigned short int status_code = this->current_client->res.get_stat_code();
    std::string res_path = get_stat_code_path(status_code);
    
    is_error_page = true;
    if (is_valid_error_path(res_path)) {
        std::cout << "------------------------------> VALID ERROR PATH <---------------------------------" << std::endl;
        this->path = res_path;
        set_header();
        set_body();
    } else {    // HERE
        std::cout << "------------------------------> NO ERROR PATH <---------------------------------" << std::endl;
        set_header();
        default_error_page(status_code);
    }
}

void    response_builder::handle_get()  // OK request
{
    std::cout << "GET REQUEST LATER ON :(" << std::endl;
    set_header();
    set_body();

    std::cout << "------- FULL RESPONSE\n" << response_holder << "\n------- FULL RESPONSE" << std::endl;
    // exit(111);
}
