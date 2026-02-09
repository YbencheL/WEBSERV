# include "../response.hpp"
# include "../socket_engine.hpp"
# include "../config_parsing/ConfigPars.hpp"

# include <sys/stat.h>
# include <dirent.h>

// REQUEST HARDCODED VALUES TO TEST
std::string method = "GET";
std::string path = "/adsf"; // most change this when you request a new path
std::string protocol = "HTTP/1.1";
std::string host = "localhost";
int port = 8080;
// --------------------------------

bool    is_allowd_method(std::deque<std::string> allow_methods, std::string method)
{
    for (size_t i = 0; i < allow_methods.size(); i++) {
        if (allow_methods.at(i) == method)
            return (true);
    }
    return (false);
}

std::string index_file_iterator(std::string &full_path ,const std::deque<std::string> &index)
{
    std::string redirection_path;
    std::string based_path = full_path;
    if (!full_path.empty() && full_path.at(full_path.length() -1) != '/')
        based_path += '/';

    for (size_t i = 0; i < index.size(); i++)
    {
        redirection_path = based_path + index.at(i);
        std::cout << "redirection_path -> " << redirection_path << std::endl;
        if (access(redirection_path.c_str(), F_OK | R_OK) == 0)
            return (redirection_path);
    }
    return ("");
}

std::string html_index_gen(std::vector<std::string> &dir_list, std::string &uri_path)
{
    std::string gen_html = "<html>\n<body>\n<h1>Index of " + uri_path + "</h1>\n<hr>\n";

    gen_html.append("<a href=\"../\">../</a><br>\n");

    for (size_t i = 0; i < dir_list.size(); i++) {
        std::string &name = dir_list.at(i);

        if (name == ".") continue;
        if (name == "..") continue;

        gen_html.append("<a href=\"" + name + "\">" + name + "</a><br>\n");
    }

    gen_html.append("<hr>\n</body>\n</html>");

    return (gen_html);
}

// autoindex: opendir, readdir and closedir.
void    autoindex_page(std::string &full_path, client &current_client)
{
    std::cout << "////////// autoindex_page call //////////" << std::endl;
    (void)current_client;
    (void)full_path;

    DIR *dir = opendir(full_path.c_str());
    if (dir == NULL)
        return (current_client.res.set_stat_code(FORBIDDEN_ACCESS), (void)0);
    std::vector<std::string> dir_list;
    while (true)
    {
        dirent *read_dir = readdir(dir);
        if (read_dir == NULL)
            break;
        dir_list.push_back(read_dir->d_name);
    }
    std::string html_gen = html_index_gen(dir_list, full_path);
    current_client.res.set_body_contnet(html_gen);
    std::cout << html_gen << std::endl;
}

std::string path_resolver(std::string root, std::string path)
{
    std::cout << "PATHHHHHHHHHHHHHHHHHHHHH -> " << path << std::endl;
    if (!root.empty() && root.at(root.length() - 1) == '/')
        root.erase(root.length() - 1);
    return (root + path);
}

// in case of the path match the ("request path" == "config path") check in local 
void    path_validation(const LocationBlock *location_block, client &current_client)
{
    struct stat statbuf;
    std::string full_path;

    // here i have to make a Path Resolver function
    full_path = path_resolver(location_block->root, location_block->path);
    if (stat(full_path.c_str(), &statbuf) < 0 || access(full_path.c_str(), R_OK) < 0) {    // existence
        current_client.res.set_stat_code(NOT_FOUND);
        return ;
    }
    if (S_ISDIR(statbuf.st_mode)) {     // is DIR
        std::string new_full_path = index_file_iterator(full_path, location_block->index);
        if (new_full_path.empty() && location_block->autoindex) {
            autoindex_page(full_path, current_client);
        } else
            current_client.res.set_stat_code(FORBIDDEN_ACCESS); // forbidden access to autoindex html page
    }
}

void    response_handler(std::deque<ServerBlock> &server_config_info, client &current_client)
{
    // get server block match
    const ServerBlock *server_req = getServerForRequest(host, port, server_config_info);
    if (server_req != NULL) {
        const LocationBlock *getLocatoin = getLocation(path, *server_req);   // get location
        if (getLocatoin != NULL) // find location
        {
            // check the method is allowd
            if (is_allowd_method(getLocatoin->allow_methods, method)) {
                // check the path is exist
                path_validation(getLocatoin, current_client);
            } else {
                current_client.res.set_stat_code(METHOD_NOT_ALLOWED);
            }
        } else {    // location not found
            current_client.res.set_stat_code(NOT_FOUND);
        }
    }
    else {
        current_client.res.set_stat_code(404);
    }
}
