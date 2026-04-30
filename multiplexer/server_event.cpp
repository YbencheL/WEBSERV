# include "../socket_engine.hpp"

void    new_connection_logs(sockaddr_in &client_addr, int &client_fd)
{
    char    *client_ip = inet_ntoa(client_addr.sin_addr);
    int     client_port = ntohs(client_addr.sin_port);
    
    std::cout << BLUE << "[Connection Log] New Connection from " << client_ip 
        << ":" << client_port << " on FD " << client_fd 
       << RSET << std::endl;
}

void socket_engine::server_event(ssize_t fd)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        std::cerr << RED << "[!] Accept failed on FD: " << fd << ": "
            << std::strerror(errno) << RSET << std::endl;
        return;
    }

    new_connection_logs(client_addr, client_fd);    // >> logs
    init_client_side(client_fd);
}
