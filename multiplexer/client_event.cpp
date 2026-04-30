# include "../socket_engine.hpp"

void    socket_engine::client_event(ssize_t fd, uint32_t events)
{
    if (events & (EPOLLHUP | EPOLLERR)) {   // >> RST -> close after (T-W H)
        terminate_client(fd, "[-] RST: network connection dropped/Error ");
        return ;
    }

    if (events & EPOLLRDHUP) {    // >> 'FIN'
        if (!(events & EPOLLIN)) {    // >> 'FIN' & 'no data'
            terminate_client(fd, "[-] Client closed connection");
            return ;
        }
    }

    if (events & EPOLLIN)	// >> Ready to read from event
        handle_epollin(fd);

    if (events & EPOLLOUT)	// >> Ready to write from event
        handle_epollout(fd);
}
