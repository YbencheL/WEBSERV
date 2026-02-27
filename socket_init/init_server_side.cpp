# include "../socket_engine.hpp"

// (DONE[*])
void socket_engine::init_server_side(std::string port, std::string host)
{

    int serv_socketFD;
    struct epoll_event new_server_ev;
    struct addrinfo hints, *result;
    
    std::memset(&new_server_ev, 0, sizeof(new_server_ev));
    std::memset (&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;          // >>> IPv
    hints.ai_socktype = SOCK_STREAM;    // >>> T/U
    hints.ai_protocol = PROTOCOL_TYLE;  // >>> (TCP/IP)

    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0)
    {
        std::cerr << "[-] Error: 'getaddrinfo' failed: " << strerror(errno) << std::endl;
        free_fds_list();
        std::exit(1);
    }

    serv_socketFD = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (serv_socketFD < 0)
    {
        std::cerr << "[-] Error: 'socket' failed: " << strerror(errno) << std::endl;
        free_fds_list();
        freeaddrinfo(result);
        std::exit(1);
    }

    new_server_ev.data.fd = serv_socketFD;  // >>> socket fd
    new_server_ev.events = EPOLLIN;         // >>> ready to read event

    int opt = 1;
    // >>> fix a problem 'bind' failed: Address already in use
    // >>> SOL_SOCKET -> change will done in socket level
    // >>> SO_REUSEADDR -> make address free witout TIME_WAIT
    if (setsockopt(serv_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(serv_socketFD);
        freeaddrinfo(result);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: 'setsockopt' failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }

    // file control(fcntl) fix the defult socket (Blocking) and make it NON_Blocking
    if (fcntl(serv_socketFD, F_SETFL, O_NONBLOCK) < 0)
    {
        freeaddrinfo(result);
        close(serv_socketFD);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: 'fcntl' failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }

    // socket binding (IP ADDRESS, ) ..  | STOP HERE
    if (bind(serv_socketFD, result->ai_addr, result->ai_addrlen) < 0)
    {
        freeaddrinfo(result);
        close (serv_socketFD);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: 'bind' failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }
    
    // >>> listening mode for any incomming Handshak
    if (listen(serv_socketFD, QUEUE_LIMIT) < 0)
    {
        freeaddrinfo(result);
        close(serv_socketFD);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: listen failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_socketFD, &new_server_ev) < 0)
    {
        freeaddrinfo(result);
        close(serv_socketFD);

        std::string errno_msg = std::strerror(errno);
        std::string error_msg = "[-] Error: 'epoll_ctl' failed: " + errno_msg;
        throw std::runtime_error(error_msg);
    }

    freeaddrinfo(result);
    set_fds_list(serv_socketFD);
    set_server_side_fds(serv_socketFD);
}
