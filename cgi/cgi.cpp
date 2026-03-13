#include "cgi.hpp"
#include "../client.hpp"
#include "../utils/utils.hpp"

size_t Cgi::CGI_MAX_OUTPUT = 10000000;

Cgi::Cgi()
{
    envp = NULL;
    argv = NULL;
}

Cgi::Cgi(const Cgi &other)
{
    *this = other;
}

Cgi &Cgi::operator=(const Cgi &other)
{
    if (this != &other)
    {
        interpreter = other.interpreter;
        extension   = other.extension;
    }
    return *this;
}

Cgi::~Cgi()
{
    if (envp)
    {
        for (size_t i = 0; envp[i]; i++)
            free(envp[i]);
        delete[] envp;
    }
    if (argv)
    {
        for (size_t i = 0; argv[i]; i++)
            free(argv[i]);
        delete[] argv;
    }
}

void Cgi::setInterpreter(const std::string &interpreter)
{
    this->interpreter = interpreter;
}

void Cgi::setExtension(const std::string &extension)
{
    this->extension = extension;
}

std::string Cgi::getInterpreter() const
{
    return interpreter;
}

std::string Cgi::getExtension() const
{
    return extension;
}

char **Cgi::getArgv() const
{
    return argv;
}

char **Cgi::getEnv() const
{
    return envp;
}

void Cgi::checkForCgi(Client &client)
{
    if (client.location_conf->cgi_handler.empty())
    {
        state = CGI_NOT_REQUIRED;
        return;
    }
    size_t dot = client.req.getPath().rfind('.');

    if (dot == std::string::npos)
    {
        state = CGI_NOT_REQUIRED;
        return;
    }
    std::string exten = client.req.getPath().substr(dot);
    std::map<std::string, std::string>::const_iterator it =
        client.location_conf->cgi_handler.begin();

    for (; it != client.location_conf->cgi_handler.end(); it++)
    {
        if (exten == it->first)
        {
            interpreter = it->second;
            extension   = it->first;
            state       = SETUP_CGI;
            return;
        }
    }
    state = CGI_NOT_REQUIRED;
}

void collectEnv(Client &client, std::vector<std::string> &env)
{
    env.push_back("REQUEST_METHOD=" + client.req.getMethod());
    env.push_back("SCRIPT_NAME=" + client.req.getPath());
    env.push_back("QUERY_STRING=" + client.req.getQuery());
    env.push_back("SERVER_PROTOCOL=" + client.req.getHttpVersion());
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_SOFTWARE=Webserve");
    env.push_back(
        "REQUEST_URI=" + client.req.getPath() + client.req.getQuery()
    );
    env.push_back("SERVER_NAME="); // hostname needed later
    env.push_back("SERVER_PORT=" + to_string(client.port));
    env.push_back("REDIRECT_STATUS=200");

    std::map<std::string, std::string> headers      = client.req.getHeaders();
    std::map<std::string, std::string>::iterator it = headers.begin();

    for (; it != headers.end(); it++)
    {

        if (it->first == "CONTENT_LENGTH" || it->first == "CONTENT_TYPE")
            env.push_back(it->first + "=" + it->second);
        else
            env.push_back("HTTP_" + it->first + "=" + it->second);
    }
}

void Cgi::buildEnv(Client &client)
{
    std::vector<std::string> env;
    size_t                   i;

    collectEnv(client, env);
    i    = env.size();
    envp = new char *[i + 1];

    for (size_t j = 0; j < i; j++)
        envp[j] = strdup(env[j].c_str());
    envp[i] = NULL;
}

void Cgi::buildArg(Client &client)
{
    argv = new char *[3];

    argv[0] = strdup(interpreter.c_str());
    argv[1] = strdup(client.req.getPath().c_str());
    argv[2] = NULL;
}

void Cgi::setupCgi(Client &client)
{
    buildEnv(client);
    buildArg(client);
    state = CREAT_PIPES;
}

void Cgi::createPipes()
{
    if (pipe(pipeIn))
    {
        std::cerr << "PIPE FAILED" << std::endl;
        state = ERROR;
        return;
    }
    if (pipe(pipeOut))
    {
        std::cerr << "PIPE FAILED" << std::endl;
        close(pipeIn[1]);
        close(pipeIn[0]);
        state = ERROR;
        return;
    }
    state = EXECUTING;
}

void Cgi::execution(Client &client)
{
    pid_t tmpid = fork();
    if (tmpid == -1)
    {
        std::cerr << "FORK FAILED" << std::endl;
        state = ERROR;
    }
    if (tmpid == 0)
        this->childProcess();
    else
    {
        pid = tmpid;
        this->parentProcess(client);
        state = CGI_READING;
    }
}

void Cgi::childProcess()
{
    dup2(pipeIn[0], STDIN_FILENO);
    dup2(pipeOut[1], STDOUT_FILENO);

    close(pipeIn[1]);
    close(pipeIn[0]);
    close(pipeOut[0]);
    close(pipeOut[1]);

    execve(argv[0], argv, envp);
    std::cerr << "EXECVE FAILED: " << strerror(errno);
    exit(1);
}

void Cgi::parentProcess(Client &client)
{
    close(pipeIn[0]);
    close(pipeOut[1]);
    if (client.parse.body)
        write(
            pipeIn[1], client.req.getBody().c_str(), client.req.getBody().size()
        );
    close(pipeIn[1]);
    gettimeofday(&start, NULL);
}

void Cgi::checkResponseAndTime()
{
    if (response.size() > CGI_MAX_OUTPUT)
    {
        kill(pid, SIGTERM);
        // todo add counter for it the killing proccess
        state = ERROR;
        return;
    }
    pid_t wait = waitpid(pid, &status, WNOHANG);
    if (wait == pid && state == CGI_WAITING)
    {
        close(pipeOut[0]);
        state = CGI_DONE;
    }
    else
    {
        gettimeofday(&current, NULL);
        if (current.tv_sec - start.tv_sec > CGI_TIMEOUT)
        {
            kill(pid, SIGTERM);
            // todo add counter for it the killing proccess
            state = ERROR;
        }
    }
}

void Cgi::reading()
{
    if (state == CGI_READING)
    {
        char buff[1024];

        int n = read(pipeOut[0], buff, 1024);
        if (n > 0)
            response.append(buff, n);
        else if (n == 0)
        {
            state = CGI_WAITING;
            close(pipeOut[0]);
        }
    }
    checkResponseAndTime();
}