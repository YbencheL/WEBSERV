# include "../socket_engine.hpp"

// (DONE[*])
void socket_engine::init_client_side(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        close (fd);
        std::cerr << "[!] fcntl failed: " << strerror(errno) << std::endl;
        return ;
    }

    struct epoll_event ev;

    std::memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = EPOLLIN; // Tell me when ready to read data from FD

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        close (fd);
        std::cerr << "[!] epoll_ctl failed: " << strerror(errno) << std::endl;
        return ;
    }
    set_fds_list(fd);
}
