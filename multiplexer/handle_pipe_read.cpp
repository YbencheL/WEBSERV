# include "../socket_engine.hpp"
# include "../response_builder.hpp"
# include "../utils/utils.hpp"

void    header_setup(Client &client, std::string &response, size_t body_size)
{
    response.append("Server: Webserv\r\n");
    response.append("Date: " + get_time() + "\r\n");
    response.append("Content-Length: " + to_string(body_size) + "\r\n");
    if (client.res.get_is_cookie_set())
    {
        const std::vector<std::string> &set_cookie_headers = client.res.get_cookie_holder();
        for (size_t i = 0; i < set_cookie_headers.size(); ++i) {
            response.append("Set-Cookie: " + set_cookie_headers[i] + "\r\n");
        }
    }
    response.append("Connection: close\r\n\r\n");
}

void    socket_engine::handle_pipe_read(int pipe_fd, uint32_t events)
{
    int client_fd = pipe_to_client[pipe_fd];
    Client &client = raw_client_data[client_fd];

    client.last_activity = time(0);

    if (client.cgiHandler.state == CGI_READY)
        client.cgiHandler.reading(events, client);

    if (client.cgiHandler.state == CGI_DONE || client.cgiHandler.state == ERROR)
    {
        int del_ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pipe_fd, NULL);
        if (del_ret == -1)
            std::cerr << "[!] epoll_ctl DEL failed: " << strerror(errno) << std::endl;
        pipe_to_client.erase(pipe_fd);
        remove_fd_from_list(pipe_fd);
        close(pipe_fd);

        if (client.cgiHandler.state == ERROR) {
            client.res.set_stat_code(SERVER_ERROR);
            response_builder rb;
            rb.init_response_builder(client);
            rb.build_response();
        } else {
            std::string &cgi_output = client.cgiHandler.getCgiResponse();
            int stat = client.cgiHandler.parseOutput(cgi_output);
            if (stat == OUTPUT_READY)
            {
                client.res.set_stat_code(OK);

                std::string response = client.res.get_start_line();
                header_setup(client, response, cgi_output.length());
                response.append(cgi_output);    // >> body

                client.res.set_raw_response(response);
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
