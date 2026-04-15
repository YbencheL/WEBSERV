# include "../socket_engine.hpp"
# include "../utils/utils.hpp"

// handle_epollout -> it's about building the response
void    socket_engine::handle_epollout(ssize_t fd)
{
    if (raw_client_data[fd].is_serving_file) // response -> stream file FD 
    {
        if (raw_client_data[fd].res.stream_response_to_client(fd))
        {
            raw_client_data[fd].close_connection = true;
            
            // TODO: move it to a new method for better readability
            std::cout << PINK << "[Response LOG] HTTP/1.0 "
                << this->raw_client_data[fd].res.get_stat_code()
                << " " << stat_code_to_string(this->raw_client_data[fd].res.get_stat_code())
                << " on FD " << fd << RSET << std::endl;

        }
    }
    else    // response -> send raw response (autoindex, error page, cgi response)
    {
        std::string &buffer = raw_client_data[fd].res.get_raw_response();
        if (!buffer.empty())
        {
            ssize_t send_stat = send(fd, buffer.c_str(), buffer.size(), MSG_NOSIGNAL);
            if (send_stat == -1)
                return ;
            if ((ssize_t)buffer.size() == send_stat)
            {
                raw_client_data[fd].close_connection = true;
                // TODO: move it to a new method for better readability
                std::cout << PINK << "[Response LOG] HTTP/1.0 "
                    << this->raw_client_data[fd].res.get_stat_code()
                    << " " << stat_code_to_string(this->raw_client_data[fd].res.get_stat_code())
                    << " on FD " << fd << RSET << std::endl;
            }
        }
        if (buffer.empty())
        {
            raw_client_data[fd].close_connection = true;
            // TODO: move it to a new method for better readability
            std::cout << PINK << "[Response LOG] HTTP/1.0 "
                << this->raw_client_data[fd].res.get_stat_code()
                << " " << stat_code_to_string(this->raw_client_data[fd].res.get_stat_code())
                << " on FD " << fd << RSET << std::endl;
        }
    }
}
