# include "../socket_engine.hpp"

void socket_engine::init_client_side(int fd)
{
    // >> add new client into map
    inisializeClient(this->raw_client_data[fd]);
    this->raw_client_data[fd].is_serving_file = false;
    this->raw_client_data[fd].last_activity = time(0);
    this->raw_client_data[fd].close_connection = false;

    struct epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    
    ev.data.fd = fd;
    
    /*
        EPOLLIN -> Tell me when ready to read data from FD
        EPOLLRDHUP -> half close (FIN)
        EPOLLHUP(full close), EPOLLERR(error happend) even not add it the kernel init that events
            by defult if it's happend
    */
    ev.events = EPOLLIN | EPOLLRDHUP;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        std::cerr << "[!] epoll_ctl EPOLL_CTL_ADD (client) failed: " << strerror(errno) << std::endl;
        close(fd);
        return ;
    }
    set_fds_list(fd);
}
