# include "./socket_engine.hpp"
# include "./config_parsing/ConfigPars.hpp"
# include "./utils/utils.hpp"

socket_engine s_engine;

# include <csignal>
void signal_handler(int sig_flag) {
    (void)sig_flag;
    // s_engine.free_fds_list();
    throw std::runtime_error("[!] SIGINT interrupt, END :(");
}

int main(int ac, char **av)
{
    signal(SIGINT, signal_handler);
    std::deque<ServerBlock> ServerConfig;

    if (ac < 2)
    {
        std::cout << "Error: file is missing" << std::endl;
        s_engine.free_fds_list();
        return 1;
    }
    std::ifstream infile(av[1]);
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

        std::string host;
        std::string port;
        for (size_t i = 0; i < ServerConfig.size(); i++)
        {
            // ----------------- JUST LOGS ----------------- //
            host = ServerConfig[i].host;
            port = to_string(ServerConfig[i].listen);
            std::cout << "Serving HTTP on " << host << " port " << port
                << " (http://" << host << ":" << port << "/)" << std::endl;
            // --------------------------------------------- //

            s_engine.init_server_side(port, host);  // TO-CHECK LATER
        }
        s_engine.process_connections(); // done [-]
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s_engine.free_fds_list();
    }
    
    return (0);
}
