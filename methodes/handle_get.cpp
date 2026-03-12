# include "../response_builder.hpp"
# include "../socket_engine.hpp"
# include "../utils/utils.hpp"

void    response_builder::set_header(void)
{
    response_holder.append(current_client->res.get_start_line());
    response_holder.append("Server: Webserv\r\n");
    response_holder.append("Date: " + get_time() + "\r\n");
    
    if (is_body_ready || is_error_page)     // TODO-CHECK: CGI later check
        response_holder.append("Content-Type: text/html\r\n");  // just in case of autoindex //
    else
        response_holder.append("Content-Type: " + extension_to_media_type(this->path) + "\r\n");
}

void    response_builder::set_body(void)
{
    /* TODO-CGI
        Before calling the: this->body = file_to_string(this->path);
        have to check if the this->path extansion is a CGI extansion
        if yes execute that file, it's resoute will be in the body
    */

    std::cout << "this->path -> " << this->path << std::endl;
    if (!is_body_ready)
        serving_static_file();
    else {
        response_holder.append("Content-Length: " + to_string(this->body_buff.size()) + "\r\n\r\n");
        response_holder.append(this->body_buff);
    }
}

void    response_builder::generate_error_page()
{
    this->is_error_page = true;
    unsigned short int status_code = this->current_client->res.get_stat_code();
    
    if (this->current_client->server_conf)
        this->path = get_stat_code_path(status_code);
    
    std::cout << "this->path -> " << this->path << std::endl;
    if (is_valid_error_path(this->path))
        serving_static_file();
    else
    {
        set_header();
        default_error_page(status_code);
    }
}

void    response_builder::handle_get()  // OK request
{
    set_header();
    set_body();
}
