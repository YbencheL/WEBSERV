# include "../response.hpp"
# include "../socket_engine.hpp"
# include "../config_parsing/ConfigPars.hpp"

# include <sys/stat.h>
# include <dirent.h>

bool    is_allowd_method(std::deque<std::string> allow_methods, std::string method)
{
    std::cout << "is_allowd_methodddddddddddddddddddddddddddddddddddddd" << std::endl;
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
void    autoindex_page(std::string &full_path, response &res)
{
    std::cout << "////////// autoindex_page call //////////" << std::endl;

    DIR *dir = opendir(full_path.c_str());
    if (dir == NULL)
        return (res.set_stat_code(FORBIDDEN_ACCESS), (void)0);
    std::vector<std::string> dir_list;
    while (true)
    {
        dirent *read_dir = readdir(dir);
        if (read_dir == NULL)
            break;
        dir_list.push_back(read_dir->d_name);
    }
    std::string html_gen = html_index_gen(dir_list, full_path);
    res.set_body_contnet(html_gen);
    std::cout << html_gen << std::endl;
    res.set_body_as_ready();
    closedir(dir);
}

std::string path_resolver(std::string root, std::string path)
{
    if (!root.empty() && root.at(root.length() - 1) == '/')
        root.erase(root.length() - 1);
    return (root + path);
}

// in case of the path match the ("request path" == "config path") check in local 
void    path_validation(const LocationBlock *location_block, response &res)
{
    struct stat statbuf;

    std::string full_path = path_resolver(location_block->root, res.get_path());
    if (stat(full_path.c_str(), &statbuf) < 0 || access(full_path.c_str(), R_OK) < 0)
        return (res.set_stat_code(NOT_FOUND), (void)0);
    if (S_ISDIR(statbuf.st_mode)) {     // is DIR
        std::string new_full_path = index_file_iterator(full_path, location_block->index);
        if (!new_full_path.empty())   // here will server the static files .html
            res.set_resolved_path(new_full_path);
        else if (new_full_path.empty() && location_block->autoindex) {
            autoindex_page(full_path, res);
        } else {
            res.set_stat_code(FORBIDDEN_ACCESS); // forbidden access to autoindex html page
        }
    }
}
