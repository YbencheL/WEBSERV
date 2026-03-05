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

// --------------------------------------------------------------------------------------------------------------

void socket_engine::check_all_client_timeouts(void) // TODO-CHECK
{
    time_t now = time(0);
    std::map<int, Client>::iterator it = raw_client_data.begin();
    
    std::cout << GREEN_S << "NUMBER of the clients -> " << raw_client_data.size() << GREEN_E << std::endl;

    while (it != raw_client_data.end())
    {
        size_t timeout_limit = TIMEOUT_LIMIT;

        int fd = it->first;
        
        if (it->second.close_connection)
        {
            epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            raw_client_data.erase(it++);
            remove_fd_from_list(fd);
            close(fd);
            // terminate_client(fd, "Response sent Successfully (HTTP/1.0)");
            std::cerr << GREEN_S <<  "+++++++++ Response sent Successfully (HTTP/1.0) +++++++++" << GREEN_E << std::endl;
            continue ;
        }

        if (it->second.server_conf != NULL)
            timeout_limit = it->second.server_conf->set_timeout;

        std::cout << GREEN_S << "\n-------- INFO START\n" << "fd:" << fd
            << "\nclose_connection: " << it->second.close_connection
            << GREEN_E << "\n-------- INFO END" <<std::endl;

        if ((now - it->second.last_activity) > (time_t)timeout_limit && !it->second.close_connection)
        {
            epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            raw_client_data.erase(it++);
            remove_fd_from_list(fd);
            close(fd);

            continue ;

            // std::cout << GREEN_S << "[-] Timeout detected on FD " << fd << ". Cleaning up..." << GREEN_E << std::endl;
        }
        else 
            ++it;
    }
}

// --------------------------------------------------------------------------------------------------------------

void    socket_engine::terminate_client(int fd, std::string stat)
{
    epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    this->raw_client_data.erase(fd);
    remove_fd_from_list(fd);
    close(fd);

    std::cerr << GREEN_S << stat << GREEN_E << std::endl;
}

void    socket_engine::modify_epoll_event(ssize_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1)
        std::cerr << "[!] epoll_ctl: " << strerror(errno) << std::endl;
}
