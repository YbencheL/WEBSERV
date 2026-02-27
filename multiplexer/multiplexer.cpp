# include "../socket_engine.hpp"
# include "../response.hpp"
# include "../response_builder.hpp"
# include "../utils/utils.hpp"
// -------------- request headers ----------------
# include "../client.hpp"
# include "../request/includes/request.hpp"
# include "../request/includes/parseRequest.hpp"
// -----------------------------------------------


// // -------------------- HARDCODED --------------------------
// // static std::string _host_ = "10.11.11.4";
// // static int _port_ = 8080;

// static std::string _host_ = "localhost";
// static int _port_ = 9090;
// // -------------------------------------------------------


void socket_engine::server_event(ssize_t fd)    // DONE
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        std::cerr << "[!] Accept failed on FD: " << fd << ": " << std::strerror(errno) << std::endl;
        return;
    }
    char    *client_ip = inet_ntoa(client_addr.sin_addr);
    int     client_port = ntohs(client_addr.sin_port);
    std::cout << "[+] New Connection from " << client_ip 
            << ":" << client_port << " on FD " << client_fd << std::endl;

    init_client_side(client_fd);
}

// WOKING ON IT
/*  TODO:
    get the Host from the request header (in case of; GET, POST or DELETE)???
    extract the host && parsing it to [host, port] then check using
        server_conf = getServerForRequest(s_host, _port_, config);
        location_conf = getLocation(this->path, *server_conf); 
    then check if the Host info is there in server block if yes serve it
    otherways error page Bad request 400
    at the end of a problem will call 
    this->raw_client_data[fd].res.set_stat_code(BAD_REQUEST);

    // --------------------------------------------------------------------------
    int s_host = address_resolution(_host_);
    server_conf = getServerForRequest(s_host, _port_, config);
    if (server_conf == NULL)    // ???
        current_client.res.set_stat_code(SERVER_ERROR);
    
    // --------------------------------------------------------------------------
    this->path = path_resolver(PATH0);
    location_conf = getLocation(this->path, *server_conf);
    if (location_conf == NULL) {
        std::cout << "THE FUCKING LOCATION PATH IS NOT EXIST IN THE SERVER BLOCK" << std::endl;
        exit(0);
        current_client.res.set_stat_code(NOT_FOUND);
    }
    // --------------------------------------------------------------------------

*/

void    socket_engine::client_event(ssize_t fd, uint32_t events)
{
    // std::cout << "[>] Data incoming from Client FD: " << fd << std::endl;   // rm-me
    
    if (events & EPOLLIN)   // READY TO READ
    {
        char raw_data[BUFFER_SIZE];
        std::memset(raw_data, 0, sizeof(raw_data));

        int recv_stat = recv(fd, raw_data, BUFFER_SIZE, 0);
        if (recv_stat > 0) {
            this->raw_client_data[fd].last_activity = time(0);
            std::string raw_data_buff(raw_data, recv_stat);
            /*  TODO-FIX
                U have to read just the header at first what ever there's a body or not
                for me a will take the header and get you witch server block matched or not
                if there's a match i'll give you back
                    client_max_body_size
                    if the method is allowd
                    the cgi
            */// --------------------------------------------------------------------------

            /*  TODO-FIX 'what i have to done' as @ma1loc
                take provided header and looking for Host
                    when i get it will splite it: addr, port
                    then will take the addr, port: search on my config file to get a match
                    if there's a match will give you back
                        client_max_body_size
                        allowd method
                        cgi
            */
            // TODO: a methode to take the raw req and extract from it info
            // -------------------------------------------------------------------------------                
            
            // ----------------- HARDCODED ------------------
            // HEADER Host -> server { ... } block to use
            // _host_, _port_, _methode -. getting it form the request
            // --------------------------------------------------------------------------

            int req_stat = parseRequest(this->raw_client_data[fd], raw_data_buff);
            std::cout << "[>] req_stat exist with it -> " << req_stat << std::endl;
            if (req_stat == REQ_NOT_READY)  // request not ready
                return ;
            else    // 200, 404, 500...
            {
                this->raw_client_data[fd].res.set_stat_code(req_stat);
                
                // JUST IN CASE HEADER IS OK
                response_builder response_builder;
                response_builder.init_response_builder(raw_client_data[fd]);
                response_builder.validate_headers(raw_client_data[fd].req.getHeaders());






                // LAST THING -------------------------------------------------------------------------------
                response_builder.build_response();
                modify_epoll_event(fd, EPOLLOUT | EPOLLIN);
                // -------------------------------------------------------------------------------
            }
        }
        else
        {
            if (recv_stat == 0)  // EOF
                terminate_client(fd, "[!] Client lost connection");
            else
                terminate_client(fd, "[!] Client connection broke");
        }
    }
    if (events & EPOLLOUT)  // READY TO WRITE
    {   
        std::string buffer_knowon = raw_client_data[fd].res.get_raw_response();
        if (!buffer_knowon.empty()) {

            ssize_t send_stat = send(fd, buffer_knowon.c_str(), buffer_knowon.size(), 0);
            if (send_stat > 0)
                buffer_knowon.erase(buffer_knowon.begin(), buffer_knowon.begin() + send_stat);
            else if (send_stat == -1)
                return ;
        }

        if (buffer_knowon.empty())
            terminate_client(fd, "Response sent Successfully (HTTP/1.0)");
    }
}

void    socket_engine::process_connections(void)
{
    while (true)
    {
        // >>> epoll_fd -> is the pipe of the fds_table in the kernal
        // >>> events -> is a array that hold the active fds
        int epoll_stat = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
        
        for (int i = 0; i < epoll_stat; i++)
        {
            // >>> get access to ready events
            int fd = events[i].data.fd;

            // >>> fd is a server fd or clietn fd
            std::vector<int>::iterator is_server = std::find(server_side_fds.begin(), server_side_fds.end(), fd);
            
            if (is_server != server_side_fds.end()) // New client + time(0)
                server_event(fd);   // DONE [*]
            else
                client_event(fd, events[i].events);   // Recv/Send + Update time(0)
        }
        
        // in this function i have to get the server config file timeout and the now - last_activity
        check_all_client_timeouts();
    }
}
