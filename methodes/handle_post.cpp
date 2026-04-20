# include "../response_builder.hpp"
# include "../utils/utils.hpp"

std::string extracting_file_name(const std::map<std::string, std::string> &header)
{
    std::string file_name = extracting_from_header(header, "FILENAME");
    if (file_name.empty())  // No file_name
    {
        std::string content_type;
        file_name = rand_str_gen();
        content_type = extracting_from_header(header, "CONTENT_TYPE");
        if (content_type.empty())   // No Content-type
            file_name += DEFAULT_EXTENSION;
        else                        // There's Content-type
            file_name += media_type_to_extension(content_type);
    }
    return (file_name);
}

std::string validate_upload_path(Client &current_client)
{
    std::string file_name = extracting_file_name(current_client.req.getHeaders());
    std::string file_path;
    std::string req_path = resolve_location_relative_path(current_client.req.getPath(),
            current_client.location_conf->path);
    file_path = join_root_path(current_client.location_conf->root, req_path);
    if (!is_dir_exist(file_path)) {
        current_client.res.set_stat_code(NOT_FOUND);
        return ("");
    }
    if (access(file_path.c_str(), X_OK | W_OK) < 0) {
        current_client.res.set_stat_code(FORBIDDEN_ACCESS);
        return ("");
    }

    file_path = join_root_path(file_path, file_name);
    
    int file_fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0) {
        current_client.res.set_stat_code(SERVER_ERROR);
        return ("");
    }
    current_client.res.set_static_file_fd(file_fd);
    return (file_path);
}

void    response_builder::handle_post()
{
    std::string file_name = validate_upload_path(*this->current_client);
    if (file_name.empty()) {
        generate_error_page();
        return ;
    }

    // >>>>>>>>>>>>>>>>>>>>>>>>> Body Processing >>>>>>>>>>>>>>>>>>>>>>>>>
    
    // is have alrady the body ready to make a response based on it
    const std::string &body_buff = this->current_client->req.getBody();
    
    if (body_buff.empty()) {    // >> NO body in the request
        // exit(123);
        this->current_client->res.set_stat_code(OK);
        this->response_holder.append(current_client->res.get_start_line());
        this->response_holder.append("Server: Webserv\r\n");
        this->response_holder.append("Date: " + get_time() + "\r\n");
        this->response_holder.append("Content-Length: 0\r\n\r\n");
        if (current_client->res.get_is_cookie_set())    // >> cookie set in the response header
        {
            std::cout << RED << "[+ handle_post] Setting cookies in response headers:" << RSET << std::endl;
            const std::vector<std::string> &set_cookie_headers = current_client->res.get_cookie_holder();

            for (size_t i = 0; i < set_cookie_headers.size(); ++i) {
                this->response_holder.append("Set-Cookie: " + set_cookie_headers[i] + "\r\n");
            }
        }
        this->response_holder.append("Content-Length: 0\r\n\r\n");  // most have to use it
        return;
    }

    ssize_t write_stat = write(this->current_client->res.get_static_file_fd(), body_buff.c_str(), body_buff.size());
    if (write_stat < 0) {
        close (this->current_client->res.get_static_file_fd());
        unlink(file_name.c_str());
        this->current_client->res.set_stat_code(SERVER_ERROR);
        return ;
    }
    close (this->current_client->res.get_static_file_fd());

    std::cout << "body -> " << this->current_client->req.getBody().empty() << std::endl;
    if (this->current_client->req.getBody().empty())
        this->current_client->res.set_stat_code(OK);
    else
        this->current_client->res.set_stat_code(CREATED);

    this->response_holder.append(current_client->res.get_start_line());
    this->response_holder.append("Server: Webserv\r\n");
    this->response_holder.append("Date: " + get_time() + "\r\n");
    if (current_client->res.get_is_cookie_set())    // >> cookie set in the response header
    {
        std::cout << RED << "[+ handle_post] Setting cookies in response headers:" << RSET << std::endl;
        const std::vector<std::string> &set_cookie_headers = current_client->res.get_cookie_holder();

        for (size_t i = 0; i < set_cookie_headers.size(); ++i) {
            this->response_holder.append("Set-Cookie: " + set_cookie_headers[i] + "\r\n");
        }
    }
    this->response_holder.append("Content-Length: 0\r\n\r\n");  // most have to use it

    std::cout << "++++ [>] POST STATUS CODE " << current_client->res.get_stat_code() << std::endl;

}
