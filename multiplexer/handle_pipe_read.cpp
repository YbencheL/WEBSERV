# include "../socket_engine.hpp"
# include "../response_builder.hpp"
# include "../utils/utils.hpp"

// it's correct
void    socket_engine::handle_pipe_read(int pipe_fd, uint32_t events)
{
    int client_fd = pipe_to_client[pipe_fd];
    Client &client = raw_client_data[client_fd];
    client.last_activity = time(0);


    /*  TODO - TO-KNOW:
        handling pipe-read with your if statment is not correct
        becouse once you read you change stat to CGI_WAITING
        that mean's your not reading again ever

        CGI_READY -> child running -> pipe events active
        by calling the handle_pipe_read() -> calling cgi.reading()
        reading():
            if bytes > 0:
                append output
            if EOF/HUP:
                set CGI_WAITING -> stdout done, child may still exit
    */
    // if i include this block
    if (client.cgiHandler.state == CGI_READY) {
        // std::cout << "readinggggggggggggggggggggg" << std::endl;
        client.cgiHandler.reading(epoll_fd, events, client);
    }
    
    // i will remove this line
    // client.cgiHandler.reading(epoll_fd, events, client);
    // std::cout << "readinggggggggggggggggggggg" << std::endl;

    if (client.cgiHandler.state == CGI_DONE || client.cgiHandler.state == ERROR)
    {
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
            rb.init_response_builder(client);
            rb.build_response();
        }
        else
        {
            // std::string cgi_output = client.cgiHandler.getCgiResponse();
            std::string &cgi_output = client.cgiHandler.getCgiResponse();

            std::string cgi_headers;
            std::string cgi_body;

            std::string sep = "\r\n\r\n";
            size_t split = cgi_output.find(sep);
            if (split == std::string::npos)
            {
                sep = "\n\n";
                split = cgi_output.find(sep);
            }

            if (split != std::string::npos)
            {
                cgi_headers = cgi_output.substr(0, split);
                cgi_body    = cgi_output.substr(split + sep.length());
            }
            else
                cgi_body = cgi_output;

            client.res.set_stat_code(OK);
            std::string response = client.res.get_start_line();
            response.append("Server: Webserv\r\n");
            response.append("Date: " + get_time() + "\r\n");
            if (!cgi_headers.empty())
                response.append(cgi_headers + "\r\n");
            response.append("Content-Length: " + to_string(cgi_body.length()) + "\r\n");
            response.append("\r\n");
            response.append(cgi_body);
            client.res.set_raw_response(response);
        }
        modify_epoll_event(client_fd, EPOLLOUT);
    }
}
