# include "../socket_engine.hpp"

// handle_epollout -> it's about building the response
void    socket_engine::handle_epollout(ssize_t fd)
{
    if (raw_client_data[fd].is_serving_file)	// response -> stream file FD 
    {
        if (raw_client_data[fd].res.stream_response_to_client(fd))
            raw_client_data[fd].close_connection = true;
    }
    else	// response -> string buffer (autoindex gen, or the defult pages that alrady small to buffer in string)
    {
        std::string buffer = raw_client_data[fd].res.get_raw_response();
        if (!buffer.empty())
        {
            ssize_t send_stat = send(fd, buffer.c_str(), buffer.size(), MSG_NOSIGNAL);
            if (send_stat == -1)
                return ;
            if ((ssize_t)buffer.size() == send_stat)
                raw_client_data[fd].close_connection = true;
        }
        if (buffer.empty())
            raw_client_data[fd].close_connection = true;
    }
}
