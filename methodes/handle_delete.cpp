# include "../response_builder.hpp"
# include "../utils/utils.hpp"

unsigned short int Delete(const std::string &root, const std::string &path)
{
    struct stat st;

    std::string full_path = root + path;
    normalisePath(full_path, "//", "/", 2);
    if (stat(full_path.c_str(), &st) != 0)
        return (NOT_FOUND);
    if (S_ISDIR(st.st_mode))
        return (FORBIDDEN_ACCESS);
    if (unlink(full_path.c_str()) == 0)
        return (NO_CONTENT);
    else
        return (SERVER_ERROR);
}

void    response_builder::handle_delete()
{
    
    std::string path = path_remainder(this->current_client->req.getPath(),
            this->current_client->location_conf->path);

    unsigned short int stat_code = Delete(current_client->location_conf->root, path);
    this->current_client->res.set_stat_code(stat_code);
    if (stat_code == NO_CONTENT)
    {
        response_holder.append(current_client->res.get_start_line());
        response_holder.append("Server: Webserv\r\n");
        response_holder.append("Date: " + get_time() + "\r\n");
        response_holder.append("Connection: close\r\n");
        response_holder.append("Content-Length: 0\r\n\r\n");
    }
    else
        generate_error_page();
}
