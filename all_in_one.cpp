# ifndef SOCKET_ENGINE_HPP
# define SOCKET_ENGINE_HPP

# include <sys/epoll.h>
# include <string>
# include <vector>
# include <map>

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <cstring>
# include <stdint.h>
# include <cerrno>
# include <cstdlib>
# include <fcntl.h>
# include <netdb.h>
# include <iostream>
# include <algorithm>
# include <deque>

# include "response.hpp"
# include "config_parsing/ConfigPars.hpp"
# include "client.hpp"

# define TIMEOUT 1000 // type???
# define TIMEOUT_LIMIT 60
# define QUEUE_LIMIT 128
# define BUFFER_SIZE 1024
# define PROTOCOL_TYLE 0
# define MAX_EVENTS 64

class socket_engine {
    private:
        int epoll_fd;   // ID for the table
        struct epoll_event events[MAX_EVENTS];
        std::vector<int> server_side_fds;   // >>> backup for the server socket fds
        std::vector<int> fds_list;  // >>> backup for all the fds used to free them in case of SIGINT
        
        std::map<int, Client> raw_client_data; // >>> raw request data stored in
        std::deque<ServerBlock> server_config_info; // >>> config file saved here

        void    server_event(ssize_t fd);
        void    client_event(ssize_t fd, uint32_t events);
        void    modify_epoll_event(ssize_t fd, uint32_t events);
        // void    handle_client_write(fd);


    public:
        socket_engine();
        void    init_client_side(int fd);
        void    init_server_side(std::string port, std::string host);

        void    process_connections(void);  // here i have to mutiplixier loop
        void    remove_fd_from_list(int fd);
        void    free_fds_list(void);
        void    check_all_client_timeouts(void);    // working on it []
        void    terminate_client(int fd, std::string stat);
        void    set_fds_list(int fd);
        void    set_server_side_fds(int s_fd);
        void    set_server_config_info(std::deque<ServerBlock> server_config);

        std::vector<int>        get_server_side_fds(void) const;
        std::map<int, Client>   &get_raw_client_data(void) const;
        const std::deque<ServerBlock> &get_server_config_info() const;
        int one_tow;
};

time_t time(time_t* timer);

# endif


#ifndef CLIENT_HPP
#define CLIENT_HPP

# include <iostream>
# include "request/includes/request.hpp"
# include "request/includes/parseRequest.hpp"
# include "response.hpp"

# define GREEN_S "\033[0;32m"
# define GREEN_E "\033[0m"

# define READ_S "\033[31m"
# define READ_E "\033[0m"

class Request;
struct reqParse;


// MAIN
// will add every thing need between [req/res] 
struct Client
{
    // this is the besed port and host
    // TODO: call = address_resolution(it->second.host);
    unsigned short int  port;
    unsigned int        host;

    /// get the location-level match
    const ServerBlock       *server_conf;
    const LocationBlock     *location_conf;

    Request req;
	reqParse parse;
    response res;
    bool    reqReady;

    // about the timeout check
    unsigned int last_activity;
};

void inisializeClient(Client &client);


#endif


# ifndef RESPONSE_BUILDER_HPP
# define RESPONSE_BUILDER_HPP

# include <deque>
# include <vector>
# include <dirent.h>
# include <sys/stat.h>
# include "response.hpp"
# include "./config_parsing/ConfigPars.hpp"
# include "./socket_engine.hpp"
# include "client.hpp"


# define GET_METHODE "GET"
# define POST_METHODE "POST"
# define DELETE_METHODE "DELETE"

struct client;
extern socket_engine s_engine;

class response_builder
{
    private:
        Client                  *current_client;
        std::string             response_holder;
        std::string             path;
        std::string             header;
        std::string             body;
        bool                    is_body_ready;
        bool                    is_error_page;
        
        bool            is_allowd_method(std::string method);
        void            path_validation(void);
        std::string     index_file_iterator(const std::string &full_path);
        void            autoindex_gen(std::vector<std::string> &dir_list, const std::string &uri_path);
        // void            autoindex_page(const std::string &full_path);
        void            autoindex_page(const std::string &full_path, const std::string &request_uri);
        void            default_error_page(unsigned short int stat_code);
        std::string     get_stat_code_path(unsigned int stat_code);
        void            generate_error_page();
        void            extract_host_info(std::string raw_req);
        void            set_header(void);
        void            set_body(void);
        void            handle_get();
        void            handle_post();
        void            handle_delete();

        void            response_setup(void);   // MAKE "response_holder"
    public:
        response_builder();
        // void    init_response_builder(Client &current_client, std::deque<ServerBlock> &config);
        void    init_response_builder(Client &current_client);
        void    validate_headers();
        void    build_response();

};

// void    validate_headers(Client &current_client);
bool    validate_headers(Client &current_client);

# endif



# include "./socket_engine.hpp"
# include "./config_parsing/ConfigPars.hpp"
# include "./utils/utils.hpp"

# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <map>
# include <string>
# include <vector>
# include "config_parsing/ConfigPars.hpp"

# define OK 200
# define BAD_REQUEST 400
# define FORBIDDEN_ACCESS 403
# define NOT_FOUND 404
# define METHOD_NOT_ALLOWED 405
# define PAYLOAD_TOO_LARGE 413
# define SERVER_ERROR 500
# define METHOD_NOT_IMPLEMENTED 501
# define VERSION_NOT_SUPP 505

# define REQ_NOT_READY 0
# define PROTOCOL_VERSION   "HTTP/1.0"

class response  // DONE[]
{
    private:
        int         port;
        std::string host;
        std::string         raw_response;   //  new

        std::string         path;
        unsigned short int  stat_code;
        bool                is_body_ready;
        
        //  USED IN THE HEADER RESPONSE
        std::string         content_type;
        ssize_t             content_length;

    public:
        response();

        // SETTERS
        void    set_stat_code(unsigned short int stat_code);
        void    set_path(std::string path);
        void    set_raw_response(std::string raw_res);

        // GETTERS
        std::string         get_str_stat_code(unsigned short int code) const;
        unsigned short int  get_stat_code(void) const;
        ssize_t             get_content_length(void) const;
        std::string         get_path(void) const;
        std::string         get_start_line(void) const;
        std::string         get_raw_response(void);

};

# endif









socket_engine s_engine;

# include <csignal>
void signal_handler(int sig_flag) {
    (void)sig_flag;
    // s_engine.free_fds_list();
    throw std::runtime_error("[!] SIGINT interrupt, END :(");
}

int main(int ac, char **av)
{
    signal(SIGINT, signal_handler);
    std::deque<ServerBlock> ServerConfig;

    if (ac < 2)
    {
        std::cout << "Error: file is missing" << std::endl;
        s_engine.free_fds_list();
        return 1;
    }
    std::ifstream infile(av[1]);
    std::string fileContent((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    if (infile.fail())
    {
        std::cout << "Error: can't open file!" << std::endl;
        s_engine.free_fds_list();
        return 1;
    }
    try
    {
        ServerConfig = tokenzation(fileContent);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s_engine.free_fds_list();
        return 1;
    }

    // --------------------- @ma1loc: MY PART START HERE ------------------------ //
    try
    {
        signal(SIGINT, signal_handler);
        s_engine.set_server_config_info(ServerConfig);

        std::string host;
        std::string port;
        for (size_t i = 0; i < ServerConfig.size(); i++)
        {
            // ----------------- JUST LOGS ----------------- //
            host = ServerConfig[i].host;
            port = to_string(ServerConfig[i].listen);
            std::cout << GREEN_S << "Serving HTTP on " << host << " port " << port
                << " (http://" << host << ":" << port << "/)"
                << GREEN_E << std::endl;
            // --------------------------------------------- //
            s_engine.init_server_side(port, host);  // TO-CHECK LATER
        }
        s_engine.process_connections(); // done [-]
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s_engine.free_fds_list();
    }
    
    return (0);
}

# include "../socket_engine.hpp"

socket_engine::socket_engine()
{
    /*  epoll_create()
        epoll_create created a table in the kernal level to save the socket fds
    */
    epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
    {
        std::cerr << "[-] Error epoll_create failed: " << strerror(errno) << std::endl; // ERR
        exit(1);
    }
}

void socket_engine::set_fds_list(int fd) {
    fds_list.push_back(fd);
}

void socket_engine::remove_fd_from_list(int fd)
{
    // rm-me
    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "[>] list befor" << std::endl;
    for (size_t i = 0; i < fds_list.size(); i++)
    {
        std::cout << "fd=" << fds_list.at(i) << " ";
    }
    std::cout << "\n";
    // rm-me

    std::vector<int>::iterator fd_position = std::find(fds_list.begin(), fds_list.end(), fd);
    if (fd_position != fds_list.end())
        fds_list.erase(fd_position);

    // rm-me
    std::cout << "[>] list after" << std::endl;
    for (size_t i = 0; i < fds_list.size(); i++)
    {
        std::cout << "fd=" << fds_list.at(i) << " ";
    }
    std::cout << "\n";
    std::cout << "--------------------------------------------------------------" << std::endl;
    // rm-me

}

void socket_engine::free_fds_list(void)
{
    for (unsigned long i = 0; i < fds_list.size(); i++)
    {
        close (fds_list.at(i));
        std::cout << ">>> free fd[" << fds_list.at(i) << "]" << std::endl;  // rm-me
    }
    std::cout << ">>> free fd[" << epoll_fd << "]" << std::endl;  // rm-me
    close (epoll_fd);
}

void socket_engine::set_server_side_fds(int s_fd) {
    server_side_fds.push_back(s_fd);
}

std::vector<int> socket_engine::get_server_side_fds(void) const {
    return (server_side_fds);
}

const std::deque<ServerBlock> &socket_engine::get_server_config_info() const {
    return (server_config_info);
}

void socket_engine::set_server_config_info(std::deque<ServerBlock> server_config_info)
{
    this->server_config_info = server_config_info;
}

void socket_engine::check_all_client_timeouts(void)
{
    time_t now = time(0);
    std::map<int, Client>::iterator it = raw_client_data.begin();

    while (it != raw_client_data.end()) 
    {
        int fd = it->first;
        int port = it->second.port;
        int host = it->second.host;
        size_t timeout_limit = TIMEOUT_LIMIT;
        
        // // ----------------------------------------------------------
        // std::cout << "it->first -> " << it->first << std::endl;
        // std::cout << "port -> " << port << std::endl;
        // std::cout << "host -> " << it->second.host << std::endl;
        // // ---------------------------------------------------------- 

        const ServerBlock *server_conf = getServerForRequest(host, port, server_config_info);
        if (server_conf != NULL)
            timeout_limit = server_conf->set_timeout;

        if ((now - it->second.last_activity) > (time_t)timeout_limit)
        {
            std::cout << "[-] Timeout detected on FD " << fd << ". Cleaning up..." << std::endl;
            epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            close(fd);
            raw_client_data.erase(it++);
            remove_fd_from_list(fd);
        }
        else 
            ++it;
    }

}

void    socket_engine::terminate_client(int fd, std::string stat)
{
    epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    this->raw_client_data.erase(fd);
    remove_fd_from_list(fd);
    close(fd);
    std::cerr << READ_S << stat << READ_E << std::endl;
}

void    socket_engine::modify_epoll_event(ssize_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1)
        std::cerr << "[!] epoll_ctl: " << strerror(errno) << std::endl;
}

# include "../socket_engine.hpp"

// (DONE[*])
void socket_engine::init_server_side(std::string port, std::string host)
{

    int serv_socketFD;
    struct epoll_event new_server_ev;
    struct addrinfo hints, *result;
    
    std::memset(&new_server_ev, 0, sizeof(new_server_ev));
    std::memset (&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;          // >>> IPv
    hints.ai_socktype = SOCK_STREAM;    // >>> T/U
    hints.ai_protocol = PROTOCOL_TYLE;  // >>> (TCP/IP)

    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0)
    {
        std::cerr << "[-] Error: 'getaddrinfo' failed: " << strerror(errno) << std::endl;
        free_fds_list();
        std::exit(1);
    }

    serv_socketFD = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (serv_socketFD < 0)
    {
        std::cerr << "[-] Error: 'socket' failed: " << strerror(errno) << std::endl;
        free_fds_list();
        freeaddrinfo(result);
        std::exit(1);
    }
    // std::cout << GREEN_S << "#2 FD -> " << serv_socketFD << GREEN_E << std::endl;


    new_server_ev.data.fd = serv_socketFD;  // >>> socket fd
    new_server_ev.events = EPOLLIN;         // >>> ready to read event

    int opt = 1;
    // >>> fix a problem 'bind' failed: Address already in use
    // >>> SOL_SOCKET -> change will done in socket level
    // >>> SO_REUSEADDR -> make address free witout TIME_WAIT
    if (setsockopt(serv_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(serv_socketFD);
        freeaddrinfo(result);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: 'setsockopt' failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }

    // file control(fcntl) fix the defult socket (Blocking) and make it NON_Blocking
    if (fcntl(serv_socketFD, F_SETFL, O_NONBLOCK) < 0)
    {
        freeaddrinfo(result);
        close(serv_socketFD);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: 'fcntl' failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }

    // socket binding (IP ADDRESS, ) ..  | STOP HERE
    if (bind(serv_socketFD, result->ai_addr, result->ai_addrlen) < 0)
    {
        freeaddrinfo(result);
        close (serv_socketFD);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: 'bind' failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }
    
    // >>> listening mode for any incomming Handshak
    if (listen(serv_socketFD, QUEUE_LIMIT) < 0)
    {
        freeaddrinfo(result);
        close(serv_socketFD);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: listen failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_socketFD, &new_server_ev) < 0)
    {
        freeaddrinfo(result);
        close(serv_socketFD);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: 'epoll_ctl' failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }

    freeaddrinfo(result);
    set_fds_list(serv_socketFD);
    set_server_side_fds(serv_socketFD);
}

# include "../socket_engine.hpp"

// (DONE[*])
void socket_engine::init_client_side(int fd)
{

    inisializeClient(this->raw_client_data[fd]);
    this->raw_client_data[fd].last_activity = time(0);

    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        close (fd);
        std::cerr << "[!] fcntl failed: " << strerror(errno) << std::endl;
        return ;
    }

    struct epoll_event ev;

    std::memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    /*
        EPOLLIN -> Tell me when ready to read data from FD
        EPOLLRDHUP -> Remote Device Hang Up ()
    */
    ev.events = EPOLLIN | EPOLLRDHUP;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        close (fd);
        std::cerr << "[!] epoll_ctl failed: " << strerror(errno) << std::endl;
        return ;
    }
    set_fds_list(fd);
}

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
    // here will parse the header infos needed //
    /*
        extract from the request header:
            Host:
                based on host will extract:
                    server-level block
                    location-level block
                        based on both:
                            will get the infos looking for
                                allowed methodes
                                max-client-body-size
                                CGI
    */
    // this->server_conf = NULL;
    current_client.location_conf = NULL;

    std::map<std::string, std::string> header = current_client.req.getHeaders();
    std::map<std::string, std::string>::iterator it = header.find("HOST");

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
            // getLocation
            current_client.server_conf = getServerForRequest(current_client.host,
                current_client.port, s_engine.get_server_config_info()); // will match server-level
            if (!current_client.server_conf) {
                current_client.res.set_stat_code(NOT_FOUND);
                return (false);
            }
            current_client.location_conf = getLocation(current_client.req.getPath(), *current_client.server_conf);
            if (!current_client.location_conf)
            {
                current_client.res.set_stat_code(NOT_FOUND);
                return (false);
            }
        }
        // this->current_client->location_conf->allow_methods
        // this->current_client->location_conf->client_max_body_size
        // this->current_client->location_conf->cgi_extension
        // this->current_client->location_conf->cgi_path
        // this->current_client->location_conf->redirection???????
    }
    else
        current_client.res.set_stat_code(NOT_FOUND);
    return (true);
}

// CGI --------------------------------------------------------------------------------------------

bool is_cgi_request(std::string path)
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

# include "../response_builder.hpp"
# include "../utils/utils.hpp"

# include "../socket_engine.hpp"    // use to use client struct

std::string   get_error_page_style()
{
    std::string error_page_style = "\n"
        "   * { padding: 0; margin: 0; }\n"
        "   body { display: flex; width: 100vw; height: 100vh; background-color: #ffffffff; }\n"
        "   .deful_page_container { display: flex; width: 100vw; height: 100vh; "
            "justify-content: center; align-items: center; flex-direction: column; "
            "font-family: \"Zalando Sans SemiExpanded\", sans-serif; }\n"
        "   .deful_page_container > * { padding-bottom: 20px; }\n"
        "   .deful_page_container h1 { font-size: 100px; }\n"
        "   .error_msg { display: flex; align-items: center; padding-bottom: 0; }\n"
        "   .go_back a { color: blue; }\n";
    return (error_page_style);
}

const std::string   get_autoindex_page_style()
{
    std::string autoindex_style = "\n"
        "   * { padding: 0; margin: 0; }\n"
        "   body { background-color: #ffffffff; padding: 40px; "
        "          font-family: \"Segoe UI\", \"Roboto\", Helvetica, Arial, sans-serif; }\n"
        "   .index_container { background: #ffffff; max-width: 800px; margin: 0 auto; "
        "                      padding: 20px; border-radius: 8px; "
        "                      box-shadow: 0 4px 10px rgba(0,0,0,0.1); }\n"
        "   h1 { border-bottom: 2px solid #eee; padding-bottom: 10px; "
        "        font-size: 24px; color: #555; }\n"
        "   a { display: block; padding: 8px 12px; text-decoration: none; "
        "       color: #0066cc; border-radius: 4px; transition: background 0.2s; }\n"
        "   a:hover { background-color: #e4e4e4ff; color: #003f8cff; }\n"
        "   .index_path { font-weight: bold; color: #ff0000; }\n"
        "   hr { border: 0; border-top: 1px solid #9a9a9a; margin: 20px 0; }\n";
    return (autoindex_style);
}

void    response_builder::default_error_page(unsigned short int stat_code)
{
    // std::string str_code = to_string(stat_code);
    const std::string status_code = to_string(stat_code);
    const std::string str_of_stat_code = stat_code_to_string(stat_code);
    const std::string title = "Webserv: " + status_code + str_of_stat_code;
    const std::string style = get_error_page_style();

    this->body = "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "\t<meta charset=\"UTF-8\">\n"
        "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "\t<title>" + title + "</title>\n"
        "\t<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n"
        "\t<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n"
        "\t<link href=\"https://fonts.googleapis.com/css2?family=Zalando+Sans+SemiExpanded:ital,wght@0,200..900;1,200..900&display=swap\" rel=\"stylesheet\">\n"
        "\t<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=sentiment_very_dissatisfied\" />"
        "\t<style>" + style + "</style>\n"
        "</head>\n"
        "<body>\n"
        "\t<div class=\"deful_page_container\">\n"
        "\t\t<h1>" + status_code + "</h1>\n"
        "\t\t<p class=\"error_code\">" + str_of_stat_code + "</p>\n"
        "\t\t<p class=\"error_msg\"> The server was unable to complete your request. <span class=\"material-symbols-outlined\"> sentiment_very_dissatisfied </span> </p>\n"
        "\t\t<p class=\"go_back\"> <a href=\"/\"> Return to Home </a> </p>\n"
        "\t</div>\n"
        "</body>\n"
        "</html>";
    this->response_holder.append("Content-Length: " + to_string(body.size()) + "\r\n\r\n");
    this->response_holder.append(this->body);
    this->is_body_ready = true;
}

void    response_builder::autoindex_gen(std::vector<std::string> &dir_list, const std::string &request_uri)
{
    std::string style = get_autoindex_page_style();
    this->body = "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "\t<meta charset=\"UTF-8\">\n"
        "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "\t<title> Index of " + request_uri + "</title>\n"
        "\t<style>" + style + "</style>\n"
        "</head>\n"
        "<body>\n"
        "\t<div class=\"index_container\">\n";

    std::cout << GREEN_S << "REQUEST PATH -> " << request_uri << GREEN_E << std::endl;
    if (request_uri != "/")
        this->body.append("\t\t<a class=\"index_path\" href=\"../\">../</a><br>\n");
    // -------------------------------------------------------------- //

    for (size_t i = 0; i < dir_list.size(); i++)
    {
        std::string &name = dir_list.at(i);
        std::string path = request_uri;

        if (name == ".") continue;
        if (name == "..") continue;

        if (request_uri.at(request_uri.length() - 1) != '/')
            path += '/';
        path += name;

        this->body.append("\t\t<a href=\"" + path + "\">" + name + "</a><br>\n");
    }
    this->body.append("\t\t<hr>\n\t</div>\n</body>\n</html>");
    this->is_body_ready = true;
}

void    response_builder::autoindex_page(const std::string &full_path, const std::string &request_uri)
{
    DIR *dir = opendir(full_path.c_str());
    if (dir == NULL){
        return (this->current_client->res.set_stat_code(FORBIDDEN_ACCESS), (void)0);
    }
    std::vector<std::string> dir_list;
    while (true)
    {
        dirent *read_dir = readdir(dir);
        if (read_dir == NULL)
            break;
        dir_list.push_back(read_dir->d_name);
    }
    autoindex_gen(dir_list, request_uri);
    closedir(dir);
}

# include "../response_builder.hpp"
# include "../socket_engine.hpp"    // used just to indelucde the client struct //  rm-me

std::string    response_builder::get_stat_code_path(unsigned int stat_code)
{
    std::deque<int> key;

    key.push_back(stat_code);
    std::map<std::deque<int>, std::string>::const_iterator it = this->current_client->server_conf->error_page.find(key);
    if(it != this->current_client->server_conf->error_page.end())
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

    this->path = join_root_path(current_client->location_conf->root, this->current_client->req.getPath());
    if (stat(path.c_str(), &statbuf) < 0) {
        this->current_client->res.set_stat_code(NOT_FOUND);
        return ;
    } else if (access(path.c_str(), R_OK) < 0)  {
        this->current_client->res.set_stat_code(FORBIDDEN_ACCESS);
        return ;
    }

    if (S_ISDIR(statbuf.st_mode)) {     // is DIR
        std::cout << "[+] DDDDDDDDDDDDDDDDDDDIR IS HERE" << std::endl;
        index = index_file_iterator(this->path);
        if (!index.empty())     // here will server the static files .html
            this->path = index;
        else if (index.empty() && current_client->location_conf->autoindex)
            autoindex_page(this->path, this->current_client->req.getPath());
        else {
            this->current_client->res.set_stat_code(FORBIDDEN_ACCESS);
        }
    }
}

# include "../response_builder.hpp"
// # include "../socket_engine.hpp"
# include "../utils/utils.hpp"
# include "../client.hpp"
# include <stack>

response_builder::response_builder(): is_body_ready(false), is_error_page(false) {};

void    response_builder::init_response_builder(Client &current_client) {
    this->current_client = &current_client;
}

bool    response_builder::is_allowd_method(std::string method)
{
    for (size_t i = 0; i < current_client->location_conf->allow_methods.size(); i++) {
        if (current_client->location_conf->allow_methods.at(i) == method)
            return (true);
    }
    return (false);
}

std::string response_builder::index_file_iterator(const std::string &full_path)
{
    std::string redirection_path;
    std::string based_path = full_path;
    if (!full_path.empty() && full_path.at(full_path.length() -1) != '/')
        based_path += '/';

    for (size_t i = 0; i < current_client->location_conf->index.size(); i++) {
        redirection_path = based_path + current_client->location_conf->index.at(i);
        if (access(redirection_path.c_str(), F_OK | R_OK) == 0)
            return (redirection_path);
    }
    return ("");
}

void response_builder::response_setup()
{
    std::cout << "STATUS CODE " << current_client->res.get_stat_code() << std::endl;
    if (current_client->res.get_stat_code() != OK)  // error page
    {
        generate_error_page();
    }
    else if (current_client->req.getMethod() == GET_METHODE)
        handle_get();
    else if (current_client->req.getMethod() == POST_METHODE)
        handle_post();
    else if (current_client->req.getMethod() == DELETE_METHODE)
        handle_delete();
    this->current_client->res.set_raw_response(response_holder);
    std::cout << "--------- START RESPONSE\n" << response_holder << "\n------- END RESPONSE" << std::endl;

}

void response_builder::build_response()
{
    if (this->current_client->res.get_stat_code() == OK) {
        if (!is_allowd_method(current_client->req.getMethod()))
            this->current_client->res.set_stat_code(METHOD_NOT_ALLOWED);
        else
            path_validation();
    }
    response_setup();
}

# include "../response.hpp"
# include "../utils/utils.hpp"

// # include <ctime>

response::response() {
    this->stat_code = 200;
    this->content_length = 0;
    this->is_body_ready = false;
    this->path = "";

};

void    response::set_stat_code(unsigned short int stat_code) {
    this->stat_code = stat_code;
}

void    response::set_path(std::string path) {
    this->path = path;
}

void    response::set_raw_response(std::string raw_res) {
    this->raw_response = raw_res;
}

unsigned short int response::get_stat_code(void) const {
    return (this->stat_code);
}

ssize_t   response::get_content_length(void) const {
    return (this->content_length);
}

std::string response::get_path(void) const {
    return (this->path);
}

std::string response::get_raw_response(void) {
    return (this->raw_response);
}


std::string response::get_start_line()const 
{
    std::string start_line_gen;

    start_line_gen.append(PROTOCOL_VERSION);
    start_line_gen.append(" ");
    start_line_gen.append(to_string(stat_code) + " ");
    start_line_gen.append(stat_code_to_string(stat_code) + "\r\n");

    return (start_line_gen);
}

# include "../socket_engine.hpp"
# include "../response.hpp"
# include "../response_builder.hpp"
# include "../utils/utils.hpp"
# include "../client.hpp"
# include "../request/includes/request.hpp"
# include "../request/includes/parseRequest.hpp"

void socket_engine::server_event(ssize_t fd)    // DONE [+]
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        std::cerr << READ_S << "[!] Accept failed on FD: " << fd << ": "
            << std::strerror(errno) << READ_E << std::endl;
        return;
    }
    char    *client_ip = inet_ntoa(client_addr.sin_addr);
    int     client_port = ntohs(client_addr.sin_port);
    std::cout << GREEN_S << "[+] New Connection from " << client_ip 
            << ":" << client_port << " on FD " << client_fd 
            << GREEN_E << std::endl;

    init_client_side(client_fd);
}

// -------------------------------------------------------------------------------

void    socket_engine::client_event(ssize_t fd, uint32_t events) // DONE []
{
    if (events & (EPOLLHUP | EPOLLERR)) {
        terminate_client(fd, "[-] RST: network connection dropped/Error ");
        return ;
    }
    if (events & EPOLLRDHUP) {    // 'FIN'
        if (!(events & EPOLLIN)) {    // 'FIN' & 'no data'
            terminate_client(fd, "[-] Client closed connection");
            return ;
        }
    }
    if (events & EPOLLIN)   // READY TO READ
    {
        
        char raw_data[BUFFER_SIZE];
        std::memset(raw_data, 0, sizeof(raw_data));

        int recv_stat = recv(fd, raw_data, BUFFER_SIZE, 0);
        if (recv_stat > 0)
        {
            this->raw_client_data[fd].last_activity = time(0);
            std::string raw_data_buff(raw_data, recv_stat);
            int req_stat = parseRequest(this->raw_client_data[fd], raw_data_buff);

            std::cout << "[>] req_stat exist with it -> " << req_stat << std::endl;
            
            if (req_stat == REQ_NOT_READY)  // request not ready
                return ;
            else if (req_stat == OK)    // header ready
            {
                // -------------------------------------------------------------------------------
                validate_headers(raw_client_data[fd]);                
                // -------------------------------------------------------------------------------
                response_builder response_builder;
                response_builder.init_response_builder(raw_client_data[fd]);

                response_builder.build_response();
                modify_epoll_event(fd, EPOLLOUT | EPOLLIN);
            }
            
            this->raw_client_data[fd].res.set_stat_code(req_stat);
        }
 
        else if (recv_stat == 0) {
            terminate_client(fd, "[!] Client lost connection (EOF)");
        }
        else
        {
            if (recv_stat == 0)  // EOF
                terminate_client(fd, "[!] Client lost connection");
            else
                terminate_client(fd, "[!] Client connection broke");
        }
    }
    if (events & EPOLLOUT)  // READY TO WRITE
    {   
        std::string buffer_knowon = raw_client_data[fd].res.get_raw_response();
        if (!buffer_knowon.empty()) {

            ssize_t send_stat = send(fd, buffer_knowon.c_str(), buffer_knowon.size(), 0);
            if (send_stat > 0)
                buffer_knowon.erase(buffer_knowon.begin(), buffer_knowon.begin() + send_stat);
            else if (send_stat == -1)
                return ;
        }

        if (buffer_knowon.empty())
            terminate_client(fd, "Response sent Successfully (HTTP/1.0)");
    }
}

void    socket_engine::process_connections(void)
{
    while (true)
    {
        // >>> epoll_fd -> is the pipe of the fds_table in the kernal
        // >>> events -> is a array that hold the active fds
        int epoll_stat = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
        for (int i = 0; i < epoll_stat; i++)
        {
            int fd = events[i].data.fd;

            // >>> fd is a server fd or clietn fd
            std::vector<int>::iterator is_server = std::find(server_side_fds.begin(), server_side_fds.end(), fd);
            
            if (is_server != server_side_fds.end()) // New client + time(0)
                server_event(fd);   // DONE [*]
            else
                client_event(fd, events[i].events);   // Recv/Send + Update time(0)
        }
        
        // in this function i have to get the server config file timeout and the now - last_activity
        check_all_client_timeouts();
    }
}

