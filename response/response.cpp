# include "../response.hpp"
# include "../utils/utils.hpp"
# include "../socket_engine.hpp"

// # include <ctime>

response::response() {
    this->stat_code = 200;
    this->content_length = 0;
    this->is_body_ready = false;
    this->path = "";

    this->static_file_fd = 0;
    this->file_size = 0;
    this->bytes_sent = 0;
};

void    response::set_stat_code(unsigned short int stat_code) {
    this->stat_code = stat_code;
}

void    response::set_path(std::string path) {
    this->path = path;
}

// void    response::set_raw_response(std::string raw_res) {
void    response::set_raw_response(std::string &raw_res) {
    this->final_raw_response = raw_res;
}

unsigned short int response::get_stat_code(void) const {
    return (this->stat_code);
}

void    response::set_static_file_fd(int fd) {
    this->static_file_fd = fd;
}

void    response::set_file_size(off_t file_size) {
    this->file_size = file_size;
}

void    response::set_bytes_sent(off_t bytes_sent) {
    this->bytes_sent = bytes_sent;
}

// -------------------------------------------------------------------

ssize_t   response::get_content_length(void) const {
    return (this->content_length);
}

std::string response::get_path(void) const {
    return (this->path);
}

// std::string response::get_raw_response(void) {
std::string &response::get_raw_response(void) {
    return (this->final_raw_response);
}


std::string response::get_start_line()const 
{
    std::string start_line_gen;

    start_line_gen.append(PROTOCOL_VERSION);
    start_line_gen.append(" ");
    start_line_gen.append(to_string(stat_code) + " ");
    start_line_gen.append(stat_code_to_string(stat_code) + "\r\n");

    return (start_line_gen);
}

int response::get_static_file_fd(void) const {
    return (this->static_file_fd);
}

off_t response::get_file_size(void) const {
    return (this->file_size);
}
off_t response::get_bytes_sent(void) const {
    return (this->bytes_sent);
}

bool    response::stream_response_to_client(int fd)
{
    if (this->bytes_sent < (off_t)final_raw_response.size())
    {
        // MSG_NOSIGNAL -> to prevent SIGPIPE signal when the client has closed the connection
        ssize_t send_stat = send(fd, final_raw_response.c_str() + this->bytes_sent, this->final_raw_response.size() - this->bytes_sent, MSG_NOSIGNAL);
        if (send_stat == -1)
            return (false);

        this->set_bytes_sent(this->bytes_sent + send_stat);
    }
    else
    {
        off_t file_offset = this->bytes_sent - final_raw_response.size();
        
        if (lseek(static_file_fd, file_offset, SEEK_SET) == (off_t)-1) {
            close(static_file_fd);
            return (true);
        }

        char file_buffer[BUFFER_SIZE];
        int readed = read(static_file_fd, file_buffer, BUFFER_SIZE);
        if (readed > 0)
        {
            // MSG_NOSIGNAL -> to prevent SIGPIPE signal when the client has closed the connection
            ssize_t bytes_actually_sent = send(fd, file_buffer, readed, MSG_NOSIGNAL);

            if (bytes_actually_sent > 0)
            {
                this->set_bytes_sent(this->bytes_sent + bytes_actually_sent);
                // - header size
                if (off_t(this->bytes_sent - final_raw_response.size()) >= this->file_size) {
                    // std::cout << "file_is_done" << std::endl;
                    close(static_file_fd);
                    return (true);
                }
            }
            else if (bytes_actually_sent == -1)
                return (false);
        }
        else if (readed == 0) {
            close(static_file_fd);
            return (true);
        }
    }
    return (false);
}
