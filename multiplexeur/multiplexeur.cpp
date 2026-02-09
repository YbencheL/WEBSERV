# include "../socket_engine.hpp"
# include "../response.hpp"

void socket_engine::process_connections(void)
{

    std::deque<ServerBlock> server_config_info = get_server_config_info();

    while (true) {
        int epoll_stat = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
        std::cout << "[>] epoll return -> " << epoll_stat << std::endl;

        for (int i = 0; i < epoll_stat; i++) {

            // access the fds of the active sockets
            int fd = events[i].data.fd;

            // int fd is a server fd or clietn fd?
            std::vector<int>::iterator is_server = std::find(server_side_fds.begin(), server_side_fds.end(), fd);
            
            if (is_server != server_side_fds.end()) {   // server event -> done[*]
                std::cout << "[>] Request incoming from Server FD: " << fd << std::endl;

                // accept is like captuer of the client fds
                int client_fd = accept(fd, NULL, NULL);
                if (client_fd != -1)
                    init_client_side(client_fd);
                else {
                    if (errno != EAGAIN && errno != EWOULDBLOCK)
                        std::cout << "[!] accept error: "<< strerror(errno) << std::endl;
                }
            }
            else    // client event -> done[]
            {
                std::cout << "[>] Data incoming from Client FD: " << fd << std::endl;

                char raw_data[BUFFER_SIZE];
                std::memset(raw_data, 0, sizeof(raw_data));

                int recv_stat = recv(fd, raw_data, BUFFER_SIZE, 0);
                std::cout << "Received " << recv_stat << " bytes from fd " << fd << std::endl;
                
                if (recv_stat > 0) {

                    std::cout << "[>] Received " << recv_stat << " bytes from fd " << fd << std::endl;
                    std::cout << "[>] --- DATA START ---\n" << raw_data << "\n--- DATA END ---" << std::endl;
                    
                    // TODO: REQ/RES
                    // ------------------------------------------------------------------------------------------ //
                    // -- HARDCODED VALUES -- request expaction
                    client &current_client = raw_client_data[fd];
                    // void    request_handler(char *raw_data, client &client);
                    // ------------------------------------------------------------------------------------------ //
                    // response expaction
                    response_handler(server_config_info ,current_client);
                    // ------------------------------------------------------------------------------------------ //
                }
                else {
                    close (fd);
                    raw_client_data.erase(fd);
                    remove_fd_from_list(fd);
                    if (recv_stat == 0)  // EOF
                        std::cerr << "[!] Client lost connection" << std::endl;
                    else
                        std::cerr << "[!] Client connection broke: " << strerror(errno) << std::endl;
                }

            }
        }
    }
}
