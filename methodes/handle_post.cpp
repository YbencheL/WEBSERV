# include "../response_builder.hpp"
# include "../utils/utils.hpp"

// unsigned short int validate_upload_path(const std::string &path) {}

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




void    response_builder::handle_post()
{
    std::string file_name = extracting_file_name(this->current_client->req.getHeaders());

    // rm-me
    std::cout << "[>] extracting_file_name -> " << file_name << std::endl;

    // ----------------------------------------------------------------------------
    std::string file_path;
    file_path = join_root_path(this->current_client->location_conf->root, this->current_client->req.getPath());
    if (!is_dir_exist(file_path)) {
        this->current_client->res.set_stat_code(NOT_FOUND);
        return ;
    }
    if (access(file_path.c_str(), X_OK | W_OK) < 0) {
        this->current_client->res.set_stat_code(FORBIDDEN_ACCESS);
        return ;
    }

    file_path = join_root_path(file_path, file_name);
    
    // i open that new file that i have to uploding the body content to it, right?
    int file_fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0) {
        this->current_client->res.set_stat_code(SERVER_ERROR);
        return ;
    }
    // ----------------------------------------------------------------------------

    // >>>>>>>>>>>>>>>>>>>>>>>>> Body Processing >>>>>>>>>>>>>>>>>>>>>>>>>
    
    // is have alrady the body ready to make a response based on it
    const std::string &body_buff = this->current_client->req.getBody();
    


    int short write_stat = write(file_fd, body_buff.c_str(), body_buff.size());
    if (write_stat < 0 || write_stat) {
        close (file_fd);
        unlink(file_name.c_str());
        this->current_client->res.set_stat_code(SERVER_ERROR);
        return ;
    }
    close (file_fd);
    this->current_client->res.set_stat_code(CREATED);

    // NO response yet

    std::cout << "[>] POST STATUS CODE " << current_client->res.get_stat_code() << std::endl;

}
