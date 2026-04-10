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

        // rm-me
        std::cout << READ_S << "--------- START REQUEST\n" << raw_data_buff << "\n------- END RAQUEST" << READ_E << std::endl;

        int req_stat = parseRequest(this->raw_client_data[fd], raw_data_buff);
        if (req_stat == REQ_NOT_READY)
            return ;

        // ------------------------------- CGI -----------------------------------
        // CGI_NOT_REQUIRED, CGI_DONE, ERROR
        this->raw_client_data[fd].cgiHandler.handleCGI(this->raw_client_data[fd]);
		if (this->raw_client_data[fd].cgiHandler.state == CGI_READY)
        {
            int pipe_out = this->raw_client_data[fd].cgiHandler.getPipeFd();   // pipeOut[0] - read CGI stdout
            int pipe_in  = this->raw_client_data[fd].cgiHandler.getPipeInFd(); // pipeIn[1]  - write body to CGI stdin

            pipe_to_client[pipe_out] = fd;
            pipe_write_to_client[pipe_in] = fd;
            set_fds_list(pipe_out);
            set_fds_list(pipe_in);

            struct epoll_event ev;
            std::memset(&ev, 0, sizeof(ev));

            ev.data.fd = pipe_out;
            ev.events  = EPOLLIN;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_out, &ev) == -1)
            {
                std::cerr << "[!] epoll_ctl EPOLL_CTL_ADD (pipe_out) failed: " << strerror(errno) << std::endl;
                terminate_client(fd, "");
                return ;
            }

		{	// this block just in case the requested cgi has body
			ev.data.fd = pipe_in;
            	ev.events  = EPOLLOUT;
            	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_in, &ev) == -1)
            	{
            		std::cerr << "[!] epoll_ctl EPOLL_CTL_ADD (pipe_in) failed: " << strerror(errno) << std::endl;
            		terminate_client(fd, "");
            		return ;
            	}
		}

            return ;
        }
        if (this->raw_client_data[fd].cgiHandler.state != CGI_NOT_REQUIRED)
            return ;
        // register pipeOut[0] in epoll (EPOLLIN)	->	pipe_to_client[pipeOut] = fd
        // register pipeIn[1]  in epoll (EPOLLOUT)	->	pipe_write_to_client[pipeIn] = fd
        // return  (don't build response yet)

        // epoll fires on pipeIn[1]  (EPOLLOUT) -> handle_pipe_write() → cgi.writing()  → write body to child stdin
        // epoll fires on pipeOut[0] (EPOLLIN)  -> handle_pipe_read()  → cgi.reading()  → read child stdout → cgi.response
        // when pipeOut[0] returns 0            -> CGI_DONE
        // → build response from cgi.response   -> modify_epoll_event(client_fd, EPOLLOUT)
        // -----------------------------------------------------------------------

        this->raw_client_data[fd].res.set_stat_code(req_stat);

        response_builder response_builder;
        response_builder.init_response_builder(raw_client_data[fd]);
        response_builder.build_response();

        modify_epoll_event(fd, EPOLLOUT | EPOLLIN);
    }
    else if (recv_stat == 0)
        terminate_client(fd, "[!] Client lost connection (EOF)");
    else
        terminate_client(fd, "[!] Client connection broke");
}
