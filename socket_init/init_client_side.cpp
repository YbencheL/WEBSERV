# include "../socket_engine.hpp"

// (DONE[*])
void socket_engine::init_client_side(int fd)
{
    inisializeClient(this->raw_client_data[fd]);
    this->raw_client_data[fd].last_activity = time(0);
    this->raw_client_data[fd].close_connection = false;

    // if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
    //     close (fd);
    //     std::cerr << "[!] fcntl failed: " << strerror(errno) << std::endl;
    //     return ;
    // }

    struct epoll_event ev;

    std::memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    /*
        EPOLLIN -> Tell me when ready to read data from FD
        EPOLLRDHUP -> Remote Device Hang Up ()
    */
    ev.events = EPOLLIN | EPOLLRDHUP;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        close (fd);
        std::cerr << "[!] epoll_ctl failed: " << strerror(errno) << std::endl;
        return ;
    }
    set_fds_list(fd);
}
