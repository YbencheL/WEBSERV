# include "../socket_engine.hpp"
# include "../response_builder.hpp"

void    socket_engine::handle_pipe_read(int pipe_fd)
{
    int client_fd = pipe_to_client[pipe_fd];		// extract client fd
    Client &client = raw_client_data[client_fd]; 	// extract client starcut
    client.last_activity = time(0);					// update the time
    std::cout << "[DEBUG] handle_pipe_read: pipe_fd=" << pipe_fd << " client_fd=" << client_fd << std::endl;

	// TODO: here we i have to call your cgi.reading

    if (client.cgiHandler.state == CGI_DONE || client.cgiHandler.state == ERROR)
    {
        std::cout << "[DEBUG] CGI done, closing pipe " << pipe_fd << " and re-adding client " << client_fd << std::endl;
        int del_ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pipe_fd, NULL);
        if (del_ret == -1)
            std::cerr << "[!] epoll_ctl DEL failed: " << strerror(errno) << std::endl;
        close(pipe_fd);
        pipe_to_client.erase(pipe_fd);
        remove_fd_from_list(pipe_fd);

        if (client.cgiHandler.state == ERROR)
        {
            client.res.set_stat_code(SERVER_ERROR);
            response_builder rb;
            rb.build_response();
        }
        else
        {
            client.res.set_stat_code(OK);
            std::cout << "[DEBUG] Building response for CGI output..." << std::endl;
        }

        modify_epoll_event(client_fd, EPOLLOUT);
    }
}
