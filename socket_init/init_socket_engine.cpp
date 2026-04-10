# include "../socket_engine.hpp"

socket_engine::socket_engine()
{
    epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
    {
        std::cerr << "[-] Error epoll_create failed: " << strerror(errno) << std::endl;
        exit(1);
    }
}

void socket_engine::set_fds_list(int fd)
{
    fds_list.push_back(fd);
}

void socket_engine::remove_fd_from_list(int fd)
{
    std::vector<int>::iterator fd_position = std::find(fds_list.begin(), fds_list.end(), fd);
    if (fd_position != fds_list.end())
        fds_list.erase(fd_position);
}

void socket_engine::free_fds_list(void)
{
    for (unsigned long i = 0; i < fds_list.size(); i++)
        close(fds_list.at(i));
    close(epoll_fd);
}

void socket_engine::set_server_side_fds(int s_fd)
{
    server_side_fds.push_back(s_fd);
}

std::vector<int> socket_engine::get_server_side_fds(void) const
{
    return (server_side_fds);
}

const std::deque<ServerBlock> &socket_engine::get_server_config_info() const
{
    return (server_config_info);
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
        size_t timeout_limit = TIMEOUT_LIMIT;
        int fd = it->first;

        if (it->second.close_connection)
        {
            ++it;
            terminate_client(fd, "");
            continue;
        }

        if (it->second.server_conf != NULL)
            timeout_limit = it->second.server_conf->set_timeout;

        if ((now - it->second.last_activity) > (time_t)timeout_limit && !it->second.close_connection)
        {
            ++it;
            terminate_client(fd, "");
            continue;
        }
        else
            ++it;
    }
}

void socket_engine::terminate_client(int fd, std::string stat)
{
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
        std::cerr << "[!] epoll_ctl EPOLL_CTL_DEL failed: " << strerror(errno) << std::endl;

    this->raw_client_data.erase(fd);
    remove_fd_from_list(fd);
    close(fd);

    if (!stat.empty())
        std::cerr << GREEN_S << "terminate_client stat ->" << stat << GREEN_E << std::endl;
}

void socket_engine::modify_epoll_event(ssize_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        terminate_client(fd, "");
        std::cerr << "[!] epoll_ctl failed with EPOLL_CTL_MOD: " << strerror(errno) << std::endl;
    }
}
