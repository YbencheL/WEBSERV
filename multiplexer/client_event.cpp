# include "../socket_engine.hpp"

void    socket_engine::client_event(ssize_t fd, uint32_t events) // DONE []
{
    if (events & (EPOLLHUP | EPOLLERR)) {
        terminate_client(fd, "[-] RST: network connection dropped/Error ");
        return ;
    }
    if (events & EPOLLRDHUP) {    // 'FIN'
        if (!(events & EPOLLIN)) {    // 'FIN' & 'no data'
            terminate_client(fd, "[-] Client closed connection");
            return ;
        }
    }
    if (events & EPOLLIN)	// ready to read from event
        handle_epollin(fd);

    if (events & EPOLLOUT)	// ready to write from event
        handle_epollout(fd);
}
