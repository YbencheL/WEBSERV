# include "../response.hpp"
# include "../utils/utils.hpp"
# include "../socket_engine.hpp"
# include "../cookies_sessions/cookies_and_sessions_logic.hpp"

response::response() {
    this->stat_code = 200;
    this->content_length = 0;
    this->is_body_ready = false;
    this->path = "";

    this->static_file_fd = 0;
    this->file_size = 0;
    this->bytes_sent = 0;
    this->is_cooke_set = false;
};

void    response::set_stat_code(unsigned short int stat_code) {
    this->stat_code = stat_code;
}

void    response::set_path(std::string path) {
    this->path = path;
}

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

void    response::save_bytes_sent(off_t bytes_sent) {
    this->bytes_sent = bytes_sent;
}

ssize_t   response::get_content_length(void) const {
    return (this->content_length);
}

std::string response::get_path(void) const {
    return (this->path);
}

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
    // TODO: check -> serving static file header first
    if (this->bytes_sent < (off_t)final_raw_response.size())    // serving 
    {
        ssize_t bytes_actually_sent = send(fd, final_raw_response.c_str() + this->bytes_sent,
            this->final_raw_response.size() - this->bytes_sent, MSG_NOSIGNAL);
        if (bytes_actually_sent == -1)
            return (false);

        this->save_bytes_sent(this->bytes_sent + bytes_actually_sent);
    }
    else    // serving the body after the header send
    {
        off_t file_offset = this->bytes_sent - final_raw_response.size();
        
        // TODO: check
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
            if (bytes_actually_sent == -1)
                return (false);
            else if (bytes_actually_sent > 0)
            {
                this->save_bytes_sent(this->bytes_sent + bytes_actually_sent);
                
                // - header size to get the size of file
                if (off_t(this->bytes_sent - final_raw_response.size()) >= this->file_size) {
                    close(static_file_fd);
                    return (true);
                }
            }
        }
        else if (readed == 0) {
            close(static_file_fd);
            return (true);
        }
    }
    return (false);
}

// about cookie and session management
// void    response::add_set_cookie_header(const std::string& header_value)
// {
//     this->is_cooke_set = true;
//     this->set_cookie_headers.push_back(header_value);
// }

bool    response::get_is_cookie_set() const
{
    return this->is_cooke_set;
}

const std::vector<std::string>& response::get_cookie_holder() const {
    return this->cookie_holder;
}

void    response::handle_session(SessionManager &session_manager, Client &client)
{
    std::string new_session_holder;
    Session& cookie = cookies_and_sessions_logic(session_manager, client);
    if(cookie.is_new) {
        std::cout << YELLOW << "[Cookie Logs] New session created with ID: " << cookie.id << RSET << std::endl;
        new_session_holder = "sessionId=" + cookie.id + "; Path=/; HttpOnly";
        this->cookie_holder.push_back(new_session_holder);
        this->is_cooke_set = true;
    }
    else
        std::cout << GREEN << "[+] Existing session accessed with ID: " << cookie.id << RSET << std::endl;
}
