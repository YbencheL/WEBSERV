# include "../socket_engine.hpp"

void    socket_engine::setup_cgi_pipes(int client_fd)
{
    int pipe_out = this->raw_client_data[client_fd].cgiHandler.getPipeOutFd();   // pipeOut[0] - read CGI stdout
    int pipe_in = this->raw_client_data[client_fd].cgiHandler.getPipeInFd();      // pipeIn[1] - write request body to CGI stdin

    pipe_to_client[pipe_out] = client_fd;
    set_fds_list(pipe_out);
    struct epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));

    ev.data.fd = pipe_out;
    ev.events  = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_out, &ev) == -1) {
        std::cerr << "[!] epoll_ctl EPOLL_CTL_ADD (pipe_out) failed: " << strerror(errno) << std::endl;
        terminate_client(client_fd, "");
        return ;
    }

    if (!this->raw_client_data[client_fd].req.getBody().empty())
    {
        pipe_write_to_client[pipe_in] = client_fd;
        set_fds_list(pipe_in);
        std::memset(&ev, 0, sizeof(ev));

        ev.data.fd = pipe_in;
        ev.events  = EPOLLOUT;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_in, &ev) == -1) {
            std::cerr << "[!] epoll_ctl EPOLL_CTL_ADD (pipe_in) failed: " << strerror(errno) << std::endl;
            terminate_client(client_fd, "");
            return ;
        }
    }
}
