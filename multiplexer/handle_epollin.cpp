# include "../socket_engine.hpp"
# include "../response.hpp"
# include "../utils/utils.hpp"
# include "../response_builder.hpp"
# include "../request/includes/parseRequest.hpp"
# include "../cookies_sessions/includes/cookies_and_sessions_logic.hpp"
# include "../cookies_sessions/includes/SessionManager.hpp"

void    socket_engine::handle_epollin(ssize_t fd)
{
    char    raw_data[BUFFER_SIZE];
    std::memset(raw_data, 0, sizeof(raw_data));
    Client  &client = this->raw_client_data[fd];

    int recv_stat = recv(fd, raw_data, BUFFER_SIZE, 0);
    if (recv_stat > 0)
    {
        client.last_activity = time(0);
        std::string raw_data_buff(raw_data, recv_stat);
        int req_stat = parseRequest(client, raw_data_buff);
        if (req_stat == REQ_NOT_READY)
            return ;

        show_request_logs(client, fd);   // >> logs
        client.res.handle_session(session_manager, client);
        client.res.set_stat_code(req_stat);
        if (client.res.get_stat_code() == OK)
        {
            client.cgiHandler.handleCGI(this->raw_client_data[fd]);
            cgiState cgi_stat = client.cgiHandler.state;
            if (cgi_stat == CGI_READY) {
                setup_cgi_pipes(fd);
                return ;
            }
        }

        response_builder response_builder;
        response_builder.init_response_builder(client);
        response_builder.build_response();
        modify_epoll_event(fd, EPOLLOUT);
    }
    else if (recv_stat == 0)
        terminate_client(fd, "[!] Client lost connection (EOF)");
    else
        terminate_client(fd, "[!] Client connection broke");
}
