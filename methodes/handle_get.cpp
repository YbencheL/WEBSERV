#include "../response_builder.hpp"
#include "../socket_engine.hpp"
#include "../utils/utils.hpp"

void response_builder::set_body(void)
{
    if (!is_body_ready) // Serv static file
        serving_static_file();
    else
    {
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

    if (is_valid_error_path(this->path))
        serving_static_file();
    else{
        set_header();
        default_error_page(status_code);
    }
}

void response_builder::set_header(void)
{
    response_holder.append(current_client->res.get_start_line());
    response_holder.append("Server: Webserv\r\n");
    response_holder.append("Date: " + get_time() + "\r\n");

    // >> Body setup will be just in case autoindex, error page.
    if (is_body_ready || is_error_page)
        response_holder.append("Content-Type: text/html\r\n");
    else
        response_holder.append("Content-Type: " + extension_to_media_type(this->path) + "\r\n");

    if (current_client->res.get_is_cookie_set())    // >> cookie set in the response header
    {
        const std::vector<std::string> &set_cookie_headers = current_client->res.get_cookie_holder();

        for (size_t i = 0; i < set_cookie_headers.size(); ++i) {
            response_holder.append("Set-Cookie: " + set_cookie_headers[i] + "\r\n");
        }
    }
    response_holder.append("Connection: close\r\n");
}

void response_builder::handle_get()
{
    set_header();   // setup header, without the content-lenght
    set_body();
}
