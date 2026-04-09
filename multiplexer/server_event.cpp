# include "../socket_engine.hpp"

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
