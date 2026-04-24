# ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <sstream>
# include <vector>

# include "../client.hpp"
# include "../socket_engine.hpp"

template <typename T>
std::string to_string(T value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

bool    					validate_headers(Client &current_client);
// const std::string           to_string(int digit);
const std::string           file_to_string(const std::string& path);
const std::string           &stat_code_to_string(unsigned short int stat_code);
const std::string           extension_to_media_type(std::string path);
const std::string           media_type_to_extension(std::string _media_type);
std::string                 get_time();
bool                        is_valid_error_path(std::string path);
std::string                 path_normalize(std::vector<std::string> path_holder);
std::string                 path_resolver(std::string request_path);
std::vector<std::string>    host_splite(std::string Host);
unsigned short int          valid_port_number(std::string port_num);
void                        dir_path_correction(const std::string &full_dir_path, std::string &d_path);
void                        normalisePath(std::string &path, std::string target, std::string rep, size_t size);
std::string                 extracting_from_header(const std::map<std::string, std::string> &header, std::string target);
std::string                 rand_str_gen();
std::string                 join_root_path(const std::string root, std::string path);
std::string                 path_remainder(const std::string &request_path, const std::string &location_path);
std::string                 resolve_request_filesystem_path(const Client &client);
bool                        is_dir_exist(const std::string &path);
bool                        is_server(std::vector<int> &server_side_fds, unsigned short int fd);
void                        show_response_logs(const Client &client, int fd);
void                        show_request_logs(const Client &client, int fd);
void                        setup_server_config_info(std::deque<ServerBlock> &ServerConfig);
void                        decode_URI(std::string &encoded_uri);

# endif

// SEGV
// (echo -e "POST /upload HTTP/1.1\r\n\r\n") | nc 10.11.11.6 8080
// (echo -e "POST /upload HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\n"; sleep 0; echo "1234567890") | nc 10.11.11.6 8080
// (echo -e "POST /upload HTTP/1.1\r\nHost: 10.11.11.6\r\nContent-Length: 10\r\n\r\n"; sleep 0; echo "1234567890") | nc 10.11.11.6 8080
// (echo -e "POST /upload HTTP/1.1\r\nHost: 10.11.11.6:8080\r\nContent-Length: 10\r\n\r\n"; sleep 0; echo "1234567890") | nc 10.11.11.6 8080

// off_t file_size -> based on content-length