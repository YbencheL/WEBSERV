# include "./socket_engine.hpp"
# include "./config_parsing/ConfigPars.hpp"
# include <csignal>

// 11-DAYS

socket_engine s_engine;

void signal_handler(int sig_flag) {
    (void)sig_flag;
    s_engine.free_fds_list();
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

    // MY PART //
    try
    {
        int num = 0;
        signal(SIGINT, signal_handler);
        s_engine.set_server_config_info(ServerConfig);

        // IN_FUNC
        for (size_t i = 0; i < ServerConfig.size(); i++)
        {
            std::cout << "setup server number #" << i+1 << std::endl;
            num = ServerConfig[i].listen;
            std::stringstream port;
            port << num;
            s_engine.init_server_side(port.str(), ServerConfig[i].host); // done[*]
        }
        s_engine.process_connections(); // done []
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s_engine.free_fds_list();
    }
    

    return (0);
}