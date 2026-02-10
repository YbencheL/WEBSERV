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

# include "request.hpp"
# include "response.hpp"
# include "config_parsing/ConfigPars.hpp"

# define TIMEOUT 1000
# define QUEUE_LIMIT 128
# define BUFFER_SIZE 1024
# define PROTOCOL_TYLE 0
# define MAX_EVENTS 64

struct client
{
    request req;
    response res;
    std::string remaining;
    bool req_ready;
};

class socket_engine {
    private:
        int epoll_fd;   // ID for the table
        struct epoll_event events[MAX_EVENTS];
        std::vector<int> server_side_fds;   // >>> backup for the server socket fds
        std::vector<int> fds_list;  // >>> backup for all the fds used to free them in case of SIGINT
        
        std::map<int, client> raw_client_data; // >>> raw request data stored in

        std::deque<ServerBlock> server_config_info; // >>> config file saved here

    public:
        socket_engine();
        void    init_client_side(int fd);
        void    init_server_side(std::string port, std::string host);

        void    process_connections(void);
        void    remove_fd_from_list(int fd);
        void    free_fds_list(void);

        void    set_fds_list(int fd);
        void    set_server_side_fds(int s_fd);
        void    set_server_config_info(std::deque<ServerBlock> server_config);

        
        std::vector<int>        get_server_side_fds(void);
        std::map<int, client>   &get_raw_client_data(void);
        std::deque<ServerBlock> get_server_config_info(void);

};

# endif