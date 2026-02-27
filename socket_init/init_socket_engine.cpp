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

    std::cout << "socket_engine successfully ready!" << std::endl;  // LOG  // rm-me
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
    close (epoll_fd);
}

void socket_engine::set_server_side_fds(int s_fd) {
    server_side_fds.push_back(s_fd);
}

std::vector<int> socket_engine::get_server_side_fds(void) {
    return (server_side_fds);
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
        size_t timeout_limit = TIMEOUT_LIMIT;
        
        // ----------------------------------------------------------
        std::cout << "it->first -> " << it->first << std::endl;
        std::cout << "port -> " << port << std::endl;
        std::cout << "host -> " << it->second.host << std::endl;
        // ---------------------------------------------------------- 


        // remove it later in the loop
        int s_host = address_resolution(it->second.host);
        // ----------------------------------------------

        const ServerBlock *server_conf = getServerForRequest(s_host, port, server_config_info);
        if (server_conf != NULL)
            timeout_limit = server_conf->set_timeout;

        if ((now - it->second.last_activity) > (time_t)timeout_limit)
        {
            std::cout << "[-] Timeout detected on FD " << fd << ". Cleaning up..." << std::endl;

            /*
                delete the fd form the epoll_fd kernal list
                why null in the case of the events
                why do i close it even the EPOLL_CTL_DEL will close it for me
            */
            epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL);

            close(fd);

            raw_client_data.erase(it++);
            remove_fd_from_list(fd);
        }
        else 
        {
            // 4. No timeout? Just move to the next one
            ++it;
        }
    }

}

void    socket_engine::terminate_client(int fd, std::string stat)
{
    epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    this->raw_client_data.erase(fd);
    remove_fd_from_list(fd);
    close(fd);
    std::cerr << stat << std::endl;
}

void    socket_engine::modify_epoll_event(ssize_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1)
        std::cerr << "[!] epoll_ctl: " << strerror(errno) << std::endl;
}
