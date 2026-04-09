# include <map>
# include <fstream>
# include <iostream>
# include <vector>
# include <algorithm>
# include <iterator>
# include <sys/stat.h>
# include <unistd.h>
# include <ctime>
# include "utils.hpp"
# include "../client.hpp"
# include "../socket_engine.hpp"

# define MIN_VALID_PORT 1024
# define MAX_VALID_PORT 65535


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
        stat_code_str[OK] = "OK";
        stat_code_str[CREATED] = "Created";
        stat_code_str[NO_CONTENT] = "No Content";
        stat_code_str[BAD_REQUEST] = "Bad Request";
        stat_code_str[FORBIDDEN_ACCESS] = "Forbidden";
        stat_code_str[NOT_FOUND] = "Not Found";
        stat_code_str[METHOD_NOT_ALLOWED] = "Method Not Allowed";
        stat_code_str[PAYLOAD_TOO_LARGE] = "Payload Too Large";
        stat_code_str[URI_TOO_LONG] = "URI Too Long";
        stat_code_str[SERVER_ERROR] = "Internal Server Error";
        stat_code_str[VERSION_NOT_SUPP] = "HTTP Version Not Supported";
        stat_code_str[HEADER_TOO_LARGE] = "Request Header Fields Too Large";
        // TODO: bad request, ETC....
    }
    return (stat_code_str[stat_code]);
}

// --------------------------------------------------------------------------------------------

const std::string extension_to_media_type(std::string path)
{
    static std::map<std::string, std::string> extensions;
    if (extensions.empty()) {
        extensions[".txt"]  = "text/plain";
        extensions[".html"] = "text/html";
        extensions[".css"] = "text/css";
        extensions[".png"] = "image/png";
        extensions[".jpg"] = "image/jpeg";
        extensions[".js"]  = "application/javascript";
        // ...
        extensions[".pdf"] = "application/pdf";
        extensions[".zip"] = "application/zip";
        extensions[".mp4"]   = "video/mp4";
        extensions[".json"]   = "application/json";
        extensions[".gif"]   = "image/gif";
    }

    size_t last_dot = path.find_last_of('.');
    if (last_dot == std::string::npos)
        return (DEFAULT_MEDIA_TYPE);    // -> "text/plain"

    
    std::string ext = path.substr(last_dot);
    for (size_t i = 0; i < ext.length(); ++i)
        ext[i] = std::tolower(ext[i]);

    std::map<std::string, std::string>::iterator it = extensions.find(ext);
    if (it != extensions.end())
        return (it->second);

    return (DEFAULT_MEDIA_TYPE);    // -> "text/plain"
}

// --------------------------------------------------------------------------------------------

const std::string media_type_to_extension(std::string _media_type)
{
    static std::map<std::string, std::string> media_type;
    if (media_type.empty()) {
        media_type["text/plain"] = ".txt";
        media_type["text/html"] = ".html";
        media_type["text/css"] = ".css";
        media_type["image/png"] = ".png";
        media_type["image/jpeg"] = ".jpg";
        media_type["application/javascript"]  = ".js";
        // ...
        media_type["application/pdf"] = ".pdf";
        media_type["application/zip"] = ".zip";
        media_type["video/mp4"] = ".mp4";
        media_type["application/json"] = ".json";
        media_type["image/gif"] = ".gif";
    }

    for (size_t i = 0; i < _media_type.length(); ++i)
        _media_type[i] = std::tolower(_media_type[i]);

    std::map<std::string, std::string>::iterator it = media_type.find(_media_type);
    if (it != media_type.end())
        return (it->second);

    return (DEFAULT_EXTENSION);     // -> ".txt"
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

    if (it == header.end()) {
        current_client.res.set_stat_code(BAD_REQUEST);
        return (false);
    }

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

        if (current_client.port != 0 && current_client.host != 0)
        {
            current_client.host_str_format = host;
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
        else {
            current_client.res.set_stat_code(BAD_REQUEST);
            return (false);
        }
    }
    else {
        current_client.res.set_stat_code(BAD_REQUEST);
        return (false);
    }
    return (true);
}


// --------------------------------------------------------------------------------------------

void    dir_path_correction(const std::string &full_dir_path, std::string &d_path)
{
    struct stat statbuf;
    
    stat(full_dir_path.c_str(), &statbuf);
    if (S_ISDIR(statbuf.st_mode) && d_path.at(d_path.size() - 1) != '/')
    {
        if (d_path.at(d_path.size() - 1) != '/')
            d_path.append("/");
    }
}

// --------------------------------------------------------------------------------------------

std::string extracting_from_header(const std::map<std::string, std::string> &header, std::string target)
{
    std::map<std::string, std::string>::const_iterator it = header.begin();
    std::map<std::string, std::string>::const_iterator ite = header.end();
    
    for ( ; it != ite; it++)
    {
        if (it->first == target)
            return (it->second);
    }
    return ("");
}

// --------------------------------------------------------------------------------------------

std::string rand_str_gen()
{
    std::srand(std::time(0));
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string random_name;
    
    unsigned short int length = 10;
    for (size_t i = 0; i < length; ++i)
    {
        int random_index = rand() % characters.length();
        random_name += characters[random_index];
    }
    
    return (random_name);
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

bool    is_server(std::vector<int> &server_side_fds, unsigned short int fd)
{
    std::vector<int>::iterator is_server = std::find(server_side_fds.begin(), server_side_fds.end(), fd);
    if (is_server != server_side_fds.end())
        return (true);
    return (false);
}