# include "../socket_engine.hpp"

void    socket_engine::handle_pipe_write(int pipe_fd, uint32_t events)
{
    int client_fd = pipe_write_to_client[pipe_fd];
    Client &client = raw_client_data[client_fd];
    client.last_activity = time(0);

    client.cgiHandler.writing(epoll_fd, events, client);
    if (client.cgiHandler.state == ERROR || client.cgiHandler.writeEnd)
    {
        pipe_write_to_client.erase(pipe_fd);
        remove_fd_from_list(pipe_fd);
    }
}
