# include "utils.hpp"
# include <map>
# include <fstream>
# include <iostream>
# include <vector>
# include <algorithm>
# include <iterator>
# include <sys/stat.h>
# include <unistd.h>


const std::string   to_string(int num)
{
    std::stringstream str;
    str << num;
    return (str.str());
}

// --------------------------------------------------------------------------------------------

const std::string   file_to_string(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
        return "";

    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();
    return (buffer.str());
}

// --------------------------------------------------------------------------------------------

const std::string   &stat_code_to_string(unsigned short int stat_code)
{
    static std::map<int, std::string> stat_code_str;
    if (stat_code_str.empty()) {
        stat_code_str[200] = "OK";
        stat_code_str[403] = "Forbidden";
        stat_code_str[404] = "Not Found";
        stat_code_str[405] = "Method Not Allowed";
        stat_code_str[413] = "Payload Too Large";
        stat_code_str[500] = "Internal Server Error";

        // TODO: bad request, ETC....
    }
    return (stat_code_str[stat_code]);
}

// --------------------------------------------------------------------------------------------

const std::string resolved_path_extension(std::string path)
{
    static std::map<std::string, std::string> extensions;
    if (extensions.empty()) {
        extensions[".txt"]  = "text/plain";
        extensions[".html"] = "text/html";
        extensions[".css"]  = "text/css";
        extensions[".png"]  = "image/png";
        extensions[".jpg"]  = "image/jpeg";
        extensions[".js"]   = "application/javascript";
    }


    size_t last_dot = path.find_last_of('.');
    if (last_dot == std::string::npos)
        return "text/plain";

    std::string ext = path.substr(last_dot);
    std::map<std::string, std::string>::iterator it = extensions.find(ext);
    if (it != extensions.end())
        return (it->second);

    return ("text/plain");
}

// --------------------------------------------------------------------------------------------

std::string get_time()
{
    char buffer[100];
    time_t now = time(0);
    struct tm *gmt_time = gmtime(&now);

    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt_time);

    return (std::string(buffer));
}

// --------------------------------------------------------------------------------------------

// check: exist, permissions, dir -> false
bool    is_valid_error_path(std::string path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) < 0
            || access(path.c_str(), F_OK | R_OK) < 0
        || S_ISDIR(statbuf.st_mode))
        return (false);
    return (true);
}

// -------------------------------------------------------------------------------------------

std::string path_normalize(std::vector<std::string> path_holder)
{
    std::string final_url;

    for (size_t i = 0; i < path_holder.size(); i++) {
        final_url.append("/");
        final_url.append(path_holder.at(i));
    }
    return (final_url);
}

// --------------------------------------------------------------------------------------------

std::string   path_resolver(std::string request_path)
{
    std::string root;
    // std::string path;

    std::vector<std::string> path_holder;
    size_t      start = 0;
    size_t      end;

    while ((end = request_path.find("/", start)) != std::string::npos)
    {
        std::string segment = request_path.substr(start, (end - start));
        if (segment == ".." && path_holder.empty()) {
            start = end + 1;
            continue;
        }
        else if (segment == "..")
            path_holder.pop_back();
        else if (!segment.empty())
            path_holder.push_back(segment);
        start = end + 1;
    }
    path_holder.push_back(request_path.substr(start));
    
    return (path_normalize(path_holder));
}

// --------------------------------------------------------------------------------------------

std::vector<std::string>    extract_host_info(std::string raw_req)
{
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, std::string> headert = raw_client_data[fd].req.getHeaders();
    for (it = headert.begin(); it != headert.end();it++)
    {
        std::cout << it->first << " :" << it->second << std::endl;
    }
}