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


        std::vector<int>        get_server_side_fds(void);
        std::map<int, Client>   &get_raw_client_data(void);
};

time_t time(time_t* timer);

# endif
