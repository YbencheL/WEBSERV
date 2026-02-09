/* >>> NEED
    Configuration file 'server'
*/

>>> To know
    Sockets type:
        Stream Sockets -> connection-oriented -> use TCP
        Datagram Sockets -> connectionless socket -> use UDP ('NOT USED')
        socket(), bind(), listen(), accept(), poll(), htons()
        Socket address family type:
            - AF_INET (IPv4)
            - AF_INET6 (IPv6)
        htons(), htonl() -> one for long other short
        Port number ?
        sockaddr set to zeros?
    is a generic container -> 'sockaddr' it can be:
        _in -> IPv4
        _in6 -> IPv6
    is an IPv4 container -> 'sockaddr_in'
    as a dev i will use sockaddr_in then cast it to 'sockaddr*'

    >>> about events:
        Request >>> POLLIN
        Response >>> POLLOUT 
        >>> POLLIN -> Alert me when data is ready to recv() on this socket.
        >>> POLLOUT -> Alert me when I can send() data to this socket without blocking.
        >>> (not used) POLLHUP -> Alert me when the remote closed the connection.

    poll syntax
        take:
            fds -> struct pollfd
            nfds -> ?
            timeout -> set a time of timeout?
        return:
            >0 -> fd of the client that ready to read it
            0 -> timeout
            -1 -> error
        int poll(struct pollfd fds[], nfds_t nfds, int timeout);

    socket syntax
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    >>> revents -> set by the kernal to know if the FD is ready to (read, write, error)
        revents holds
            0: not-ready
            POLLIN → ready to read
            POLLOUT → ready to write
            POLLERR → error occurred
            POLLHUP → connection closed / hang up
            POLLNVAL → invalid fd
    INADDR_ANY?

/* >>> TODO
    TODO: create listening sockets, for browser listening
    TODO: build the sockets in server side for a client
    TODO: Webserv it most listening on multiple ports
        - every webserv listening port has it's -> [socket(), bind(), listen()]
*/

/*
    socket(), send(), recv(), htons()
*/ 


>>> TODO: check about revents:
    POLLIN >>> Data is here >>> Call recv() or accept().
    POLLOUT >>> Pipe is clear >>> Call send().
    POLLERR >>>The socket broke. >>> close() it and remove from vector.
    POLLHUP >>> Client closed the tab. >>> close() it and remove from vector.
    POLLNVAL >>> FD is not open. >>> This usually means you have a bug in your code.
