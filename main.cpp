# include "./socket_engine.hpp"
# include "./config_parsing/includes/ConfigPars.hpp"
# include "./cookies_sessions/SessionManager.hpp"
# include "./utils/utils.hpp"
# include <csignal>
#include <sys/time.h>

socket_engine s_engine;

void signal_handler(int sig_flag) {
    (void)sig_flag;
    const char msg[] = "\033[3;43;30m\n[!] SIGINT received, shutting down gracefully...\033[0m";
    throw std::runtime_error(msg);
}

int main(int ac, char **av)
{
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, signal_handler);
    std::deque<ServerBlock> ServerConfig;
    std::string fileName;
    timeval t1;

    gettimeofday(&t1, NULL);
    std::srand(t1.tv_sec * 1000 + t1.tv_usec / 1000); // Seed the random number generator with current time in milliseconds
    if (ac < 2)
        fileName = "./config.conf";
    else
        fileName = av[1];
    std::ifstream infile(fileName.c_str());
    std::string fileContent((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    if (infile.fail())
    {
        std::cout << "Error: can't open file!" << std::endl;
        s_engine.free_fds_list();
        return 1;
    }
    try
    {
        ServerConfig = tokenzation(fileContent);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s_engine.free_fds_list();
        return 1;
    }

    // --------------------- @ma1loc: MY PART START HERE ------------------------ //
    try
    {
        signal(SIGINT, signal_handler);
        s_engine.set_server_config_info(ServerConfig);
        
        // ----------------- SERVER LOGS ----------------- //
        setup_server_config_info(ServerConfig);  // S_Logs
        s_engine.process_connections();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s_engine.free_fds_list();
    }
    return (0);
}
