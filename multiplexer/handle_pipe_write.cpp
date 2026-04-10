# include "../socket_engine.hpp"

// NOTE: it's not done yet
void    socket_engine::handle_pipe_write(int pipe_fd)
{
    int client_fd = pipe_write_to_client[pipe_fd];
    Client &client = raw_client_data[client_fd];
    client.last_activity = time(0);

	// NOTE: here will start writing the req-body to CGI
    
    // If body fully sent close stdin of CGI
	// based on the content-length, but in case of chunk it's your choice
    // if (client.cgiHandler.sent >= body.size())
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pipe_fd, NULL) == -1)
            std::cerr << "[!] epoll_ctl EPOLL_CTL_DEL (pipe_write) failed: " << strerror(errno) << std::endl;
        close(pipe_fd);
        pipe_write_to_client.erase(pipe_fd);
        remove_fd_from_list(pipe_fd);
    }
}
