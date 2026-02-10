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

    std::cout << "socket_engine successfully ready!" << std::endl;  // LOG
}

void socket_engine::set_fds_list(int fd) {
    fds_list.push_back(fd);
}

void socket_engine::remove_fd_from_list(int fd)
{

    std::cout << "--------------------------------------------------------------" << std::endl;
    std::cout << "[>] list befor" << std::endl;
    for (size_t i = 0; i < fds_list.size(); i++)
    {
        std::cout << "fd=" << fds_list.at(i) << " ";
    }
    std::cout << "\n";

    std::vector<int>::iterator fd_position = std::find(fds_list.begin(), fds_list.end(), fd);
    if (fd_position != fds_list.end())
        fds_list.erase(fd_position);


    std::cout << "[>] list after" << std::endl;
    for (size_t i = 0; i < fds_list.size(); i++)
    {
        std::cout << "fd=" << fds_list.at(i) << " ";
    }
    std::cout << "\n";

    std::cout << "--------------------------------------------------------------" << std::endl;
}

void socket_engine::free_fds_list(void)
{
    for (unsigned long i = 0; i < fds_list.size(); i++)
    {
        close (fds_list.at(i));
        std::cout << ">>> free fd[" << fds_list.at(i) << "]" << std::endl;
    }
    close (epoll_fd);
}

void socket_engine::set_server_side_fds(int s_fd) {
    server_side_fds.push_back(s_fd);
}

std::vector<int> socket_engine::get_server_side_fds(void) {
    return (server_side_fds);
}

std::deque<ServerBlock> socket_engine::get_server_config_info(void)
{
    return (server_config_info);
}

void socket_engine::set_server_config_info(std::deque<ServerBlock> server_config_info)
{
    this->server_config_info = server_config_info;
}