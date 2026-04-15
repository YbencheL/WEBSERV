# include "../socket_engine.hpp"
# include "../utils/utils.hpp"

void socket_engine::process_connections(void)   // main func about events
{
    while (true)
    {
        int epoll_stat = epoll_wait(epoll_fd, events, EPOLL_MAX_EVENTS, EPOLL_TIMEOUT);
        if (epoll_stat == -1) {
            std::cerr << "[!] epoll_wait failed: " << strerror(errno) << std::endl;
            continue ;
        }
        for (int i = 0; i < epoll_stat; i++)
        {
            int fd = events[i].data.fd;
            if (is_server(server_side_fds, fd)) // new event record
                server_event(fd);
            else if (pipe_to_client.count(fd))	// ready to read from CGI
                handle_pipe_read(fd, events[i].events);
            else if (pipe_write_to_client.count(fd))    // ready to write to CGI
                handle_pipe_write(fd, events[i].events);
            else
                client_event(fd, events[i].events); // client event
        }
        check_all_client_timeouts();    // timeout check
    }
}
