# include "../socket_engine.hpp"
# include "../response.hpp"
# include "../response_builder.hpp"
# include "../request/includes/parseRequest.hpp"

void    socket_engine::handle_epollin(ssize_t fd)
{
    char raw_data[BUFFER_SIZE];
    std::memset(raw_data, 0, sizeof(raw_data));

    /*  TODO: FIX
        here i start recving data from the client bytes by bytes
        resiving the request form the client, but buffring all at once
        then build the response, it's not a good idea for lot or requests
        - to fix that most buffer full header
            #1 -> finish reading the header, if header valid -> stream the body
                else the header no valid i will expacted from you "status code" of that error:
                    BAD_REQUEST
                    HEADER_TOO_LARGE
                    METHOD_NOT_ALLOWED
                    PAYLOAD_TOO_LARGE
                    URI_TOO_LONG
                    ...
                    you can find the macros in -> response.hpp
            #2 -> if the header request is valid stream the body by buffring
                in a std::string BUFFER_SIZE flag that has just 8192
            #3 -> in case the reading the body has any problem about it.
                like:
                    - content-length in the header = 100MB
                        but the body client total size has 200MB
                        we will not nowing that intel reading the >100MB
                        the drop the request with error
                        becouse we stream every chunk of the body we can not
                        send a response of PAYLOAD_TOO_LARGE or somting like that
                        becouse we alrady stream it, rigth?
                        is that problme will cassing just in case of CGI only right?
                            i was thinking about POST methode that has 100Mb and it's not a CGI is i alrady handel that?
                    - timeout check
                        case of havin a while loop with or without an output must be checked
    */

    // TODO: non-CGI FIX
    // For non-CGI POST requests, stream the request body (do not buffer full body in memory).
    // Everything else in the non-CGI flow is correct.

    int recv_stat = recv(fd, raw_data, BUFFER_SIZE, 0);
    if (recv_stat > 0)
    {
        this->raw_client_data[fd].last_activity = time(0);
        std::string raw_data_buff(raw_data, recv_stat);

        // rm-me
        std::cout << READ_S << "--------- START REQUEST\n" << raw_data_buff << "\n------- END RAQUEST" << READ_E << std::endl;

        // what i will expact for the parse request
        int req_stat = parseRequest(this->raw_client_data[fd], raw_data_buff);
        if (req_stat == REQ_NOT_READY)
            return ;

        this->raw_client_data[fd].res.set_stat_code(req_stat);

        response_builder rb;
        rb.init_response_builder(raw_client_data[fd]);
        rb.resolve_request_path();
        
        // std::cout << "parseRequest req -> " << this->raw_client_data[fd].req.getPath() << std::endl;
        // std::cout << "parseRequest res -> " << this->raw_client_data[fd].res.get_path() << std::endl;
        // exit(1);

        // ------------------------------- CGI -----------------------------------
        // CGI_NOT_REQUIRED, CGI_DONE, ERROR
        if (this->raw_client_data[fd].res.get_stat_code() == OK)
            this->raw_client_data[fd].cgiHandler.handleCGI(this->raw_client_data[fd]);
		// -----------------------------------------------------------------------

        if (this->raw_client_data[fd].cgiHandler.state == CGI_READY)
        {
            int pipe_out = this->raw_client_data[fd].cgiHandler.getPipeOutFd();   // pipeOut[0] - read CGI stdout
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

        // response
        response_builder response_builder;
        response_builder.init_response_builder(raw_client_data[fd]);
        response_builder.build_response();
        modify_epoll_event(fd, EPOLLOUT);
        // modify_epoll_event(fd, EPOLLOUT | EPOLLIN);
    }
    else if (recv_stat == 0)
        terminate_client(fd, "[!] Client lost connection (EOF)");
    else
        terminate_client(fd, "[!] Client connection broke");
}
