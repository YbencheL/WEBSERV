# include "../response_builder.hpp"
# include "../utils/utils.hpp"

bool    is_dir_exist(const std::string &path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {

        return (true);
    }
    return (false);
}

std::string    response_builder::get_stat_code_path(unsigned int stat_code)
{
    std::map<int, std::string>::const_iterator it = this->current_client->server_conf->error_page.find(stat_code);
    if(it != this->current_client->server_conf->error_page.end())
        return (it->second);
    return ("");
}


std::string join_root_path(const std::string root, std::string path)
{
    std::string final_url = root;
    if (!final_url.empty() && final_url.at(final_url.length() - 1) == '/')
        final_url.erase(final_url.length() - 1);
    
    if (!path.empty() && path.at(0) != '/')
        final_url += "/";
    
    final_url += path;
    normalisePath(final_url, "//", "/", 2);
    normalisePath(final_url, "/./", "/", 3);
    return (final_url);
}

// TODO: check
std::string path_remainder(const std::string &request_path, const std::string &location_path)
{
    std::string path_after_location = request_path;
    int req_size = request_path.size();
    int loc_size = location_path.size();

    if (req_size >= loc_size
        && path_after_location.substr(0, location_path.size()) == location_path)
            path_after_location = path_after_location.substr(location_path.size());

    if (path_after_location.empty())
        path_after_location = "/";
    return (path_after_location);
}

std::string resolve_request_filesystem_path(const Client &client)
{
    if (client.location_conf == NULL)
        return ("");

    std::string req_path = path_remainder(
        client.req.getPath(),
        client.location_conf->path
    );
    return (join_root_path(client.location_conf->root, req_path));
}

void    response_builder::return_handling()
{
    std::map<int, std::string>::const_iterator it = 
        current_client->location_conf->redirection.begin();

    this->current_client->res.set_stat_code(it->first);
    response_holder.clear();
    response_holder.append(current_client->res.get_start_line());
    response_holder.append("Server: Webserv\r\n");
    response_holder.append("Date: " + get_time() + "\r\n");
    if (!it->second.empty())    // location
        response_holder.append("Location: " + it->second + "\r\n");
    response_holder.append("Connection: close\r\n");

    if (current_client->res.get_is_cookie_set())    // >> cookie set in the response header
    {
        const std::vector<std::string> &set_cookie_headers = current_client->res.get_cookie_holder();
        for (size_t i = 0; i < set_cookie_headers.size(); ++i) {
            response_holder.append("Set-Cookie: " + set_cookie_headers[i] + "\r\n");
        }
    }
    response_holder.append("Content-Length: 0\r\n\r\n");
}

void    response_builder::path_validation()
{
    std::string index;
    struct stat statbuf;

    if (current_client->server_conf == NULL || current_client->location_conf == NULL) {
        this->current_client->res.set_stat_code(SERVER_ERROR);
        return ;
    }
	
    this->path = resolve_request_filesystem_path(*this->current_client);

	if (!this->current_client->location_conf->redirection.empty()) {
        return_handling();
        return ;
    }

    if (stat(this->path.c_str(), &statbuf) < 0) {
        this->current_client->res.set_stat_code(NOT_FOUND);
        return ;
    } else if (access(this->path.c_str(), R_OK) < 0)  {
        this->current_client->res.set_stat_code(FORBIDDEN_ACCESS);
        return ;
    }
    if (S_ISDIR(statbuf.st_mode))
    {
        index = index_file_iterator(this->path);
        if (!index.empty()) // >> if index file exist in the directory
            this->path = index;
        else if (index.empty() && current_client->location_conf->autoindex)
            autoindex_page(this->path, this->current_client->req.getPath());
        else {
            // >> autoindex(off) && no index file -> 404 not found
            if (!this->current_client->location_conf->autoindex
                && this->current_client->req.getMethod() == "GET")
                this->current_client->res.set_stat_code(NOT_FOUND);
        }
    }
}
