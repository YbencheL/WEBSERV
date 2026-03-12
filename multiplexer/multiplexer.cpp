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


            std::cout << READ_S << "--------- START REQUEST\n" << raw_data_buff << "\n------- END RAQUEST" << READ_E << std::endl;

            int req_stat = parseRequest(this->raw_client_data[fd], raw_data_buff);
            if (req_stat == REQ_NOT_READY)
                return ;

            this->raw_client_data[fd].res.set_stat_code(req_stat);

            // -------------------------------------------------------------------------------

            response_builder response_builder;

            response_builder.init_response_builder(raw_client_data[fd]);

            response_builder.build_response();
            
            modify_epoll_event(fd, EPOLLOUT | EPOLLIN);
            // exit(1);

            // -------------------------------------------------------------------------------

        }
        else if (recv_stat == 0)
            terminate_client(fd, "[!] Client lost connection (EOF)");
        else
            terminate_client(fd, "[!] Client connection broke");
    }

    if (events & EPOLLOUT)
    {
        if (raw_client_data[fd].is_serving_file)
        {
            // exit(5);
            std::cout << GREEN_S << "+++++++++++++++++++++++++++++++++ YOUR SERVING STATIC FILE +++++++++++++++++++++++++++++++++" << GREEN_E << std::endl;
            off_t bytes_send = raw_client_data[fd].res.get_bytes_sent();
            int file_fd = raw_client_data[fd].res.get_static_file_fd();
            off_t file_total_size = raw_client_data[fd].res.get_file_size();
            size_t header_size = raw_client_data[fd].res.get_raw_response().size();

            if (bytes_send < (off_t)header_size) {
                std::string buffer = raw_client_data[fd].res.get_raw_response();
                // ssize_t send_stat = send(fd, buffer.c_str() + bytes_send, buffer.size() - bytes_send, 0);
                ssize_t send_stat = send(fd, buffer.c_str() + bytes_send, buffer.size() - bytes_send, MSG_NOSIGNAL);
                
                if (send_stat == -1) return; 
                
                raw_client_data[fd].res.set_bytes_sent(bytes_send + send_stat);
            }


            else {
                off_t file_offset = bytes_send - header_size;
                
                if (lseek(file_fd, file_offset, SEEK_SET) == (off_t)-1) {
                    close(file_fd);
                    raw_client_data[fd].close_connection = true;
                    return;
                }

                char file_buffer[BUFFER_SIZE];
                int readed = read(file_fd, file_buffer, BUFFER_SIZE);
                
                if (readed > 0) {
                    // ssize_t bytes_actually_sent = send(fd, file_buffer, readed, 0);
                    ssize_t bytes_actually_sent = send(fd, file_buffer, readed, MSG_NOSIGNAL);
                    if (bytes_actually_sent > 0) {
                        raw_client_data[fd].res.set_bytes_sent(bytes_send + bytes_actually_sent);
                        
                        if (off_t(bytes_send + bytes_actually_sent - header_size) >= file_total_size) {
                            close(file_fd);
                            raw_client_data[fd].close_connection = true;
                        }
                    }
                    else if (bytes_actually_sent == -1)
                        return ;
                }
                else if (readed == 0) {
                    close(file_fd);
                    raw_client_data[fd].close_connection = true;
                }
            }
        }
        else {
            // exit(6);
            std::string buffer = raw_client_data[fd].res.get_raw_response();
            if (!buffer.empty())
            {
                ssize_t send_stat = send(fd, buffer.c_str(), buffer.size(), MSG_NOSIGNAL);
                if (send_stat == -1)
                    return ;
                if ((ssize_t)buffer.size() == send_stat)
                    raw_client_data[fd].close_connection = true;
            }
            if (buffer.empty())
                    raw_client_data[fd].close_connection = true;
        }
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
