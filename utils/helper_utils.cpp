# include <map>
# include <fstream>
# include <iostream>
# include <vector>
# include <algorithm>
# include <iterator>
# include <sys/stat.h>
# include <unistd.h>
# include "utils.hpp"
# include "../client.hpp"
# include "../socket_engine.hpp"

# define MIN_VALID_PORT 1024
# define MAX_VALID_PORT 65535

const std::string   to_string(int digit)
{
    std::stringstream str;
    str << digit;
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
        stat_code_str[204] = "No Content";
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

unsigned short int  valid_port_number(std::string port_num)
{
    for (unsigned int i = 0; i < port_num.size(); i++) {
        if (!std::isdigit(port_num[i])) {
            return (0);
        }
    }
    unsigned int port = std::atoi(port_num.c_str());
    if (port <= MIN_VALID_PORT || port > MAX_VALID_PORT)
        return (0);
    return (port);
}

// --------------------------------------------------------------------------------------------

extern socket_engine s_engine;
bool    validate_headers(Client &current_client)
{
    std::map<std::string, std::string> header = current_client.req.getHeaders();
    std::map<std::string, std::string>::iterator it = header.find("HOST");

    current_client.server_conf = NULL;
    current_client.location_conf = NULL;

    const unsigned long index = it->second.find(":");
    if (index != std::string::npos)
    {
        std::string host = it->second.substr(0, index);
        std::string port = it->second.substr((index + 1));

        current_client.host = address_resolution(host);
        if (current_client.host == INADDR_NONE)  // invalid host
            current_client.host = 0;

        current_client.port = valid_port_number(port);
        if (!current_client.port)    // invalid port
            current_client.port = 0;

        if (current_client.port != 0 && current_client.host != INADDR_NONE)
        {
            current_client.config_file_info.setServerForRequest(current_client.host,
                current_client.port, s_engine.get_server_config_info());
            current_client.server_conf = current_client.config_file_info.getServer();
            if (!current_client.server_conf) {
                current_client.res.set_stat_code(NOT_FOUND);
                return (false);
            }
            current_client.location_conf = current_client.config_file_info.getLocation(current_client.req.getPath());
            if (!current_client.location_conf)
            {
                current_client.res.set_stat_code(NOT_FOUND);
                return (false);
            }
        }
    }
    else
        current_client.res.set_stat_code(NOT_FOUND);
    return (true);
}

// CGI --------------------------------------------------------------------------------------------

bool    is_cgi_request(std::string path)
{
    size_t last_dot = path.find_last_of('.');
    if (last_dot == std::string::npos)
        return false;

    std::string ext = path.substr(last_dot);
    if (ext == ".py" || ext == ".php")
        return true; 
    return false;
}

// --------------------------------------------------------------------------------------------

void    dir_path_correction(const std::string &full_dir_path, std::string &d_path)
{
    struct stat statbuf;
    
    stat(full_dir_path.c_str(), &statbuf);
    if (S_ISDIR(statbuf.st_mode) && d_path.at(d_path.size() - 1) != '/') {
        if (d_path.at(d_path.size() - 1) != '/')
            d_path.append("/");
    }
}
