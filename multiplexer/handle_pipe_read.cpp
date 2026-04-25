# include "../socket_engine.hpp"
# include "../response_builder.hpp"
# include "../utils/utils.hpp"

// it's correct
void    socket_engine::handle_pipe_read(int pipe_fd, uint32_t events)
{
    int client_fd = pipe_to_client[pipe_fd];
    Client &client = raw_client_data[client_fd];
    client.last_activity = time(0);

    if (client.cgiHandler.state == CGI_READY)
        client.cgiHandler.reading(events, client);

    if (client.cgiHandler.state == CGI_DONE || client.cgiHandler.state == ERROR)
    {
        // TODO: check
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
        else {   // TODO: check
            std::string &cgi_output = client.cgiHandler.getCgiResponse();
            int stat = client.cgiHandler.parseOutput(cgi_output);
            if (stat == OUTPUT_READY)
            {
                // todo: check
                client.res.set_stat_code(OK);

                // ---------------------------------------------------------------------------

                // std::cout << "cgi lenght: " << cgi_output.length() << std::endl;
                // std::cout << "cgi output:\n" << cgi_output << std::endl;
                // exit(1);
                // ---------------------------------------------------------------------------


                // ---------------------------------------------------------------------------
                std::string response = client.res.get_start_line();
                response.append("Server: Webserv\r\n");
                response.append("Date: " + get_time() + "\r\n");
                response.append("Content-Length: " + to_string(cgi_output.length()) + "\r\n");
                response.append("Connection: close\r\n");
                response.append("\r\n");
                response.append(cgi_output);
                client.res.set_raw_response(response);
                // ---------------------------------------------------------------------------
            }
            else if (stat == INTERNAL_SERVER_ERROR)
            {
                client.res.set_stat_code(INTERNAL_SERVER_ERROR);
                response_builder rb;
                rb.init_response_builder(client);
                rb.build_response();
            }
        }
        modify_epoll_event(client_fd, EPOLLOUT);
    }
}
