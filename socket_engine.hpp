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
# include <netdb.h>
# include <iostream>
# include <algorithm>
# include <deque>
#include <fcntl.h>

# include "response.hpp"
# include "config_parsing/includes/ConfigPars.hpp"
# include "./cookies_sessions/includes/SessionManager.hpp"
# include "client.hpp"


// >> epoll_wait timeout in milliseconds (1000 ms = 1 second)
# define EPOLL_TIMEOUT 1000

// >> max number of ready events returned in one epoll_wait call
# define EPOLL_MAX_EVENTS 64

// >> listen backlog: max pending TCP connections before accept()
# define SOCK_SERVER_QUEUE_LIMIT 128

// >> fallback idle timeout per client in seconds
//      (can be overridden by server block set_timeout)
# define TIMEOUT_LIMIT 5

// >> recv/read chunk size in bytes
# define BUFFER_SIZE 1048576
// # define BUFFER_SIZE 8192

// >> default protocol for socket/getaddrinfo
//      (0 lets OS choose protocol matching SOCK_STREAM)
# define PROTOCOL_TYLE 0

class socket_engine {
    private:
        int epoll_fd;
        struct epoll_event events_holder[EPOLL_MAX_EVENTS];
        std::vector<int> server_side_fds;
        std::vector<int> fds_list;

        std::map<int, Client> raw_client_data;
        std::deque<ServerBlock> server_config_info;

		std::map<int, int> pipe_to_client;
		std::map<int, int> pipe_write_to_client;

        SessionManager session_manager;

        void    server_event(ssize_t fd);
        void    client_event(ssize_t fd, uint32_t events);
        void    handle_epollin(ssize_t fd);
        void    handle_epollout(ssize_t fd);
        void    setup_cgi_pipes(int client_fd);
        void    handle_pipe_read(int pipe_fd, uint32_t events);
        void    handle_pipe_write(int pipe_fd, uint32_t events);
        void    modify_epoll_event(ssize_t fd, uint32_t events);

    public:
        socket_engine();
        void    init_client_side(int fd);
        void    init_server_side(std::string port, std::string host);

        void    process_connections(void);
        void    remove_fd_from_list(int fd);
        void    free_fds_list(void);
        void    timeout_monitoring(void);
        void    terminate_client(int fd, std::string stat);
        void    set_fds_list(int fd);
        void    set_server_side_fds(int s_fd);
        void    set_server_config_info(std::deque<ServerBlock> server_config);

        std::vector<int>        get_server_side_fds(void) const;
        std::map<int, Client>   &get_raw_client_data(void) const;
        const std::deque<ServerBlock> &get_server_config_info() const;
};

# endif
