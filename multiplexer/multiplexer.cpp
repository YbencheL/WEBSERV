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

            if (is_server(server_side_fds, fd))
                server_event(fd);
            else if (pipe_to_client.count(fd))	// ready to read from CGI
                handle_pipe_read(fd);
			// ready to write body to CGI (chunk by chunk)
            else if (pipe_write_to_client.count(fd))
                handle_pipe_write(fd);
            else
                client_event(fd, events[i].events);
        }
        check_all_client_timeouts();
    }
}
