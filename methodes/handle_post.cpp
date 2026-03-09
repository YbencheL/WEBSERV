# include "../response_builder.hpp"
# include "../utils/utils.hpp"
#include <fcntl.h>
// unsigned short int validate_upload_path(const std::string &path)

void    response_builder::handle_post()
{

    // >>>>>>>>>>>>>>>>>>>>>>>>> Header Processing >>>>>>>>>>>>>>>>>>>>>>>>>
    std::string file_name;
    std::string content_type;

    file_name = extracting_from_header(this->current_client->req.getHeaders(), "FILENAME");
    if (file_name.empty())  // No file_name
    {
        file_name = rand_str_gen();
        content_type = extracting_from_header(this->current_client->req.getHeaders(), "CONTENT_TYPE");
        if (content_type.empty())   // No Content-type
            file_name += DEFAULT_EXTENSION;
        else                        // There's Content-type
            file_name += media_type_to_extension(content_type);

    }
    
    std::string file_path;
    file_path = join_root_path(this->current_client->location_conf->root, this->current_client->req.getPath());
    
    std::cout << "file_path: " << file_path << std::endl;

    if (!is_dir_exist(file_path)) {
        this->current_client->res.set_stat_code(NOT_FOUND);
        return ;
    }
    file_path = join_root_path(file_path, file_name);
    if (access(file_path.c_str(), X_OK | W_OK) < 0) {
        this->current_client->res.set_stat_code(FORBIDDEN_ACCESS);
        return ;
    }

    int fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        this->current_client->res.set_stat_code(SERVER_ERROR);
        return ;
    }

    // >>>>>>>>>>>>>>>>>>>>>>>>> Body Processing >>>>>>>>>>>>>>>>>>>>>>>>>
    this->current_client->req.getBody();
    

}
