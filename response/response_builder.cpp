# include "../response_builder.hpp"
# include "../utils/utils.hpp"
# include "../client.hpp"
# include <stack>

response_builder::response_builder(): is_body_ready(false), is_error_page(false) {};

void    response_builder::init_response_builder(Client &current_client) {
    this->current_client = &current_client;
}

void    response_builder::resolve_request_path()
{
    if (this->current_client->res.get_stat_code() != OK)
        return;
    path_validation();
    this->current_client->res.set_path(this->path);
}

std::string response_builder::index_file_iterator(const std::string &full_path)
{
    std::string routing_path;
    std::string based_path = full_path;
    if (!full_path.empty() && full_path.at(full_path.length() -1) != '/')
        based_path += '/';

    std::set<std::string>::const_iterator it = current_client->location_conf->index.begin();
    for ( ; it != current_client->location_conf->index.end(); it++)
    {
        routing_path = based_path + *it;
        if (access(routing_path.c_str(), F_OK | R_OK) == 0)
            return (routing_path);
    }
    return ("");
}

void    response_builder::set_static_file_res_header(off_t content_length)
{
    this->header_buff.append(current_client->res.get_start_line());
    this->header_buff.append("Server: Webserv\r\n");
    this->header_buff.append("Date: " + get_time() + "\r\n");
    if (is_error_page)
        this->header_buff.append("Content-Type: text/html\r\n");
    else
        this->header_buff.append("Content-Type: " + extension_to_media_type(this->path) + "\r\n");\

    if (current_client->res.get_is_cookie_set())    // >> cookie set in the response header
    {
        const std::vector<std::string> &set_cookie_headers = current_client->res.get_cookie_holder();

        for (size_t i = 0; i < set_cookie_headers.size(); ++i) {
            this->header_buff.append("Set-Cookie: " + set_cookie_headers[i] + "\r\n");
        }
    }
    this->header_buff.append("Connection: close\r\n");
    this->header_buff.append("Content-Length: " + to_string(content_length) + "\r\n\r\n");
}

void    response_builder::serving_static_file()
{
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
    this->current_client->res.set_file_size(st.st_size);

    set_static_file_res_header(st.st_size);
    this->response_holder = header_buff;
}

void response_builder::build_response()
{
    if (this->current_client->res.get_stat_code() != OK)
        generate_error_page();
    else
    {
        resolve_request_path();  // >> auto-index gen
        int stat = this->current_client->res.get_stat_code();
        if (stat >= 300 && stat < 400)

            ;
        else if (stat != OK)
            generate_error_page();
        else
        {
            if (this->current_client->req.getMethod() == GET_METHODE)
                handle_get();

            else if (this->current_client->req.getMethod() == POST_METHODE)
                handle_post();

            else if (this->current_client->req.getMethod() == DELETE_METHODE)
                handle_delete();
        }

    }
    this->current_client->res.set_raw_response(response_holder);
}
