# include "../socket_engine.hpp"
# include "../response.hpp"
# include "../response_builder.hpp"
# include "../request/includes/parseRequest.hpp"

void    socket_engine::handle_epollin(ssize_t fd)
{
    char raw_data[BUFFER_SIZE];
    std::memset(raw_data, 0, sizeof(raw_data));

    int recv_stat = recv(fd, raw_data, BUFFER_SIZE, 0);
    if (recv_stat > 0)
    {
        this->raw_client_data[fd].last_activity = time(0);
        std::string raw_data_buff(raw_data, recv_stat);
        int req_stat = parseRequest(this->raw_client_data[fd], raw_data_buff);
        if (req_stat == REQ_NOT_READY)
            return ;
        
        // TODO: move this log to a new method for better readability
        // std::string request_log(client &client, int fd);
        std::cout << YELLOW << "[Reqest LOG] " << this->raw_client_data[fd].req.getMethod()
                << " " << this->raw_client_data[fd].req.getPath()
                << " " << this->raw_client_data[fd].req.getHttpVersion()
                << " on FD " << fd << RSET << std::endl;

        this->raw_client_data[fd].res.set_stat_code(req_stat);
        if (this->raw_client_data[fd].res.get_stat_code() == OK)
            this->raw_client_data[fd].cgiHandler.handleCGI(this->raw_client_data[fd]);

        // TODO: hold it in a new method for better readability
        if (this->raw_client_data[fd].cgiHandler.state == CGI_READY)
        {
            int pipe_out = this->raw_client_data[fd].cgiHandler.getPipeOutFd();   // pipeOut[0] - read CGI stdout
            int pipe_in = this->raw_client_data[fd].cgiHandler.getPipeInFd();      // pipeIn[1] - write request body to CGI stdin

            pipe_to_client[pipe_out] = fd;
            set_fds_list(pipe_out);

            struct epoll_event ev;
            std::memset(&ev, 0, sizeof(ev));
            ev.data.fd = pipe_out;
            ev.events  = EPOLLIN;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_out, &ev) == -1) {
                std::cerr << "[!] epoll_ctl EPOLL_CTL_ADD (pipe_out) failed: " << strerror(errno) << std::endl;
                terminate_client(fd, "");
                return ;
            }

            // TO FIX LATER WITH A BOOL FLAG -> req.getBody().empty()
            if (!this->raw_client_data[fd].req.getBody().empty())
            {
                pipe_write_to_client[pipe_in] = fd;
                set_fds_list(pipe_in);
                std::memset(&ev, 0, sizeof(ev));
                ev.data.fd = pipe_in;
                ev.events  = EPOLLOUT;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_in, &ev) == -1) {
                    std::cerr << "[!] epoll_ctl EPOLL_CTL_ADD (pipe_in) failed: " << strerror(errno) << std::endl;
                    terminate_client(fd, "");
                    return ;
                }
            }
            return ;
        }

        // Only skip if CGI is actively in progress (past the initial CHECKING state)
        cgiState cgi_stat = this->raw_client_data[fd].cgiHandler.state;
        if (cgi_stat != CGI_NOT_REQUIRED && cgi_stat != CHECKING)
            return ;    // CGI is handling this request

        response_builder response_builder;
        response_builder.init_response_builder(raw_client_data[fd]);
        response_builder.build_response();
        modify_epoll_event(fd, EPOLLOUT);
    }
    else if (recv_stat == 0)
        terminate_client(fd, "[!] Client lost connection (EOF)");
    else
        terminate_client(fd, "[!] Client connection broke");
}
