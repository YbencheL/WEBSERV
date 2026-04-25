# include "../socket_engine.hpp"
# include "../utils/utils.hpp"

void socket_engine::process_connections(void)   // main func about events
{
    while (true)
    {
        /*  TODO-KNOW:
            EPOLL_MAX_EVENTS -> if epoll_wait has no event will return 0 each EPOLL_TIMEOUT
            EPOLL_MAX_EVENTS -> max event that epoll_wait can done each loop
            events_holder -> is a place holder of the active FDs event
            epoll_fd -> is the main epoll table that hold the fds in the background (SERVER/CLIETN[FDS])
        */
        int epoll_stat = epoll_wait(epoll_fd, events_holder, EPOLL_MAX_EVENTS, EPOLL_TIMEOUT);
        if (epoll_stat == -1) {
            std::cerr << "[!] epoll_wait failed: " << strerror(errno) << std::endl;
            continue ;
        }
        for (int i = 0; i < epoll_stat; i++)
        {
            int fd = events_holder[i].data.fd;
            // >>> new event record
            if (is_server(server_side_fds, fd))
                server_event(fd);
            // >>> ready to read from CGI
            else if (pipe_to_client.count(fd))
                handle_pipe_read(fd, events_holder[i].events);
            // >>> ready to write to CGI
            else if (pipe_write_to_client.count(fd))
                handle_pipe_write(fd, events_holder[i].events);
            // >>> client event
            else
                client_event(fd, events_holder[i].events);
        }
        timeout_monitoring();
    }
}
