# include "../response_builder.hpp"
# include "../socket_engine.hpp"    // used just to indelucde the client struct //  rm-me

std::string    response_builder::get_stat_code_path(unsigned int stat_code)
{
    std::deque<int> key;

    key.push_back(stat_code);
    std::map<std::deque<int>, std::string>::const_iterator it = server_conf->error_page.find(key);
    if(it != server_conf->error_page.end())
        return (it->second);
    return ("");
}

std::string join_root_path(const std::string root, std::string path)
{
    std::string final_url = root;
    if (!final_url.empty() && final_url.at(final_url.length() - 1) == '/')
        final_url.erase(final_url.length() - 1);

    if (!path.empty() && path.at(0) != '/') {
        final_url += "/";
    }

    final_url += path;
    return (final_url);
}

void    response_builder::path_validation()
{
    struct stat statbuf;
    std::string index;

    this->path = join_root_path(location_conf->root, this->path);
    if (stat(path.c_str(), &statbuf) < 0) {
        this->current_client->res.set_stat_code(NOT_FOUND);
        return ;
    } else if (access(path.c_str(), R_OK) < 0)  {
        this->current_client->res.set_stat_code(FORBIDDEN_ACCESS);
        return ;
    }

    if (S_ISDIR(statbuf.st_mode)) {     // is DIR
        std::cout << "[+] DDDDDDDDDDDDDDDDDDDIR IS HERE" << std::endl;
        index = index_file_iterator(path);
        if (!index.empty())     // here will server the static files .html
            this->path = index;
        else if (index.empty() && this->location_conf->autoindex)
            autoindex_page(this->path);   // STOP HERE //
        else {
            this->current_client->res.set_stat_code(FORBIDDEN_ACCESS);
        }
    }
}
