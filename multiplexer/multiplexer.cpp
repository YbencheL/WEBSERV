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

            // std::cout << "-----REQUEST " << raw_data_buff << "-----REQUEST " << std::endl;

            int req_stat = parseRequest(this->raw_client_data[fd], raw_data_buff);
            if (req_stat == REQ_NOT_READY)  // request not ready
                return ;
            else if (req_stat == OK)    // request ready
            {
                validate_headers(raw_client_data[fd]);

                // -------------------------------------------------------------------------------
                std::cout << READ_S << "--------- START REQUEST\n" << raw_data_buff << "\n------- END RAQUEST" << READ_E << std::endl;


                response_builder response_builder;
                response_builder.init_response_builder(raw_client_data[fd]);
                response_builder.build_response();
                modify_epoll_event(fd, EPOLLOUT | EPOLLIN);
                // -------------------------------------------------------------------------------
            }
            this->raw_client_data[fd].res.set_stat_code(req_stat);
        }
        else if (recv_stat == 0)
            terminate_client(fd, "[!] Client lost connection (EOF)");
        else
            terminate_client(fd, "[!] Client connection broke");
    }
    if (events & EPOLLOUT)  // READY TO WRITE
    {  
        // std::cout << "=================================================================== inter ========================================================" << std::endl;
        std::string buffer_knowon = raw_client_data[fd].res.get_raw_response();
        if (!buffer_knowon.empty())
        {
            ssize_t send_stat = send(fd, buffer_knowon.c_str(), buffer_knowon.size(), 0);
            if (send_stat > 0)
                buffer_knowon.erase(buffer_knowon.begin(), buffer_knowon.begin() + send_stat);
            else if (send_stat == -1)
                return ;
        }

        if (buffer_knowon.empty())
            raw_client_data[fd].close_connection = true;
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
        check_all_client_timeouts();
    }
}
