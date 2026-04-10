#include "cgi.hpp"
#include "../client.hpp"
#include "../utils/utils.hpp"
#include <fcntl.h>

size_t Cgi::CGI_MAX_OUTPUT = 1000000000;

Cgi::Cgi()
{
    sent        = 0;
    safeExit    = true;
    writeEnd    = false;
    closedAll   = false;
    sigTermSent = false;
    envp        = NULL;
    argv        = NULL;
}

Cgi::Cgi(const Cgi &other)
{
    *this = other;
}

char **deepCopy(char **src)
{
    if (!src)
        return NULL;

    int i = 0;
    for (i = 0; src[i]; i++)
    {
    }
    char **copy = new char *[i + 1];
    for (i = 0; src[i]; i++)
        copy[i] = strdup(src[i]);
    copy[i] = NULL;
    return copy;
}

Cgi &Cgi::operator=(const Cgi &other)
{
    if (this != &other)
    {
        interpreter     = other.interpreter;
        extension       = other.extension;
        envp            = deepCopy(other.envp);
        argv            = deepCopy(other.argv);
        pipeIn[0]       = other.pipeIn[0];
        pipeIn[1]       = other.pipeIn[1];
        pipeOut[0]      = other.pipeOut[0];
        pipeOut[1]      = other.pipeOut[1];
        response        = other.response;
        state           = other.state;
        pid             = other.pid;
        status          = other.status;
        start.tv_sec    = other.start.tv_sec;
        start.tv_usec   = other.start.tv_usec;
        current.tv_sec  = other.current.tv_sec;
        current.tv_usec = other.current.tv_usec;
        sent            = other.sent;
        writeEnd        = other.writeEnd;
        safeExit        = other.safeExit;
        closedAll       = other.closedAll;
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
    scriptPath = client.res.get_path();

    size_t dot = scriptPath.rfind('.');

    if (dot == std::string::npos)
    {
        state = CGI_NOT_REQUIRED;
        return;
    }
    std::string exten = scriptPath.substr(dot);
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

    char *abs = realpath(client.res.get_path().c_str(), NULL);
    if (abs)
    {
        env.push_back("SCRIPT_FILENAME=" + std::string(abs));
        free(abs);
    }
    else
        env.push_back("SCRIPT_FILENAME=" + client.res.get_path());

    env.push_back("PATH_INFO=" + client.req.getPath());

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

void Cgi::buildArg()
{
    argv = new char *[3];

    argv[0] = strdup(interpreter.c_str());
    argv[1] = strdup(scriptPath.substr(scriptPath.rfind('/') + 1).c_str());
    argv[2] = NULL;
}

void Cgi::setupCgi(Client &client)
{
    buildEnv(client);
    buildArg();
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
    fcntl(pipeIn[1], F_SETFL, O_NONBLOCK);
    fcntl(pipeOut[0], F_SETFL, O_NONBLOCK);

    state = EXECUTING;
}

void Cgi::execution(Client &client)
{
    pid_t tmpid = fork();
    if (tmpid == -1)
    {
        std::cerr << "FORK FAILED" << std::endl;
        state = ERROR;
        return;
    }
    if (tmpid == 0)
        this->childProcess();
    else
    {
        pid = tmpid;
        this->parentProcess(client);
        state = CGI_READY; // this state mean the cgi is ready to pass fds for
                           // epoll event
    }
}

void Cgi::childProcess()
{
    std::string scriptDir = scriptPath.substr(0, scriptPath.rfind('/'));

    if (!scriptDir.empty())
        chdir(scriptDir.c_str());

    if (dup2(pipeIn[0], STDIN_FILENO) == -1 ||
        dup2(pipeOut[1], STDOUT_FILENO) == -1)
    {
        perror("dup2 failed :");

        close(pipeIn[1]);
        close(pipeIn[0]);
        close(pipeOut[0]);
        close(pipeOut[1]);

        exit(1);
    }

    close(pipeIn[1]);
    close(pipeIn[0]);
    close(pipeOut[0]);
    close(pipeOut[1]);

    execve(argv[0], argv, envp);
    perror("execve failed :");
    exit(1);
}

void Cgi::parentProcess(Client &client)
{
    close(pipeIn[0]);
    close(pipeOut[1]);
    gettimeofday(&start, NULL);
}

void Cgi::closeEverything(int epoll_fd, Client &client)
{
    if (!closedAll)
    {
        if (client.parse.body && !writeEnd)
        {
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pipeIn[1], NULL);
            close(pipeIn[1]);
        }
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pipeOut[0], NULL);
        close(pipeOut[0]);
        closedAll = true;
    }
}

void Cgi::writing(int epoll_fd, unsigned int events, Client &client)
{
    if (!(events & EPOLLOUT))
        return;

    if (state != CGI_READY)
        return;
    std::string &body = client.req.getBody();

    size_t remaining = body.size() - sent;

    if (remaining == 0)
    {
        writeEnd = true;
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pipeIn[1], NULL);
        close(pipeIn[1]);
        return;
    }

    size_t chunk = remaining;
    if (chunk > WRITE_READ_LIMIT)
        chunk = WRITE_READ_LIMIT;

    ssize_t written = write(pipeIn[1], body.c_str() + sent, chunk);

    if (written > 0)
    {
        sent += written;
        if (sent == body.size())
        {
            writeEnd = true;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pipeIn[1], NULL);
            close(pipeIn[1]);
        }
    }

    if (written == -1)
    {
        closeEverything(epoll_fd, client);
        state = ERROR;
    }
}

void Cgi::reading(int epoll_fd, unsigned int events, Client &client)
{
    checkResponseAndTime(epoll_fd, client);

    if (!(events & EPOLLIN) && !(events & EPOLLHUP))
        return;

    char buff[WRITE_READ_LIMIT];

    int n = read(pipeOut[0], buff, WRITE_READ_LIMIT);

    if (n > 0)
        response.append(buff, n);
    else if (n == 0)
    {
        closeEverything(epoll_fd, client);
        state = CGI_WAITING;
    }
    else if (n == -1)
    {
        closeEverything(epoll_fd, client);
        state = ERROR;
    }
}

void Cgi::checkResponseAndTime(int epoll_fd, Client &client)
{
    pid_t wait = waitpid(pid, &status, WNOHANG);
    if (wait == pid && sigTermSent)
    {
        state = ERROR;
        return ;
    }
    gettimeofday(&current, NULL);
    if (wait == pid && state == CGI_WAITING && safeExit)
        state = CGI_DONE;
    else
    {
        if (current.tv_sec - start.tv_sec > CGI_TIMEOUT)
        {
            if (!sigTermSent)
            {
                closeEverything(epoll_fd, client);
                safeExit = false;
                kill(pid, SIGTERM);
                sigTermSent = true;
                wait        = waitpid(pid, &status, WNOHANG);
                if (wait == pid)
                    state = ERROR;
            }
            else
            {
                gettimeofday(&current, NULL);
                if ((current.tv_sec - start.tv_sec) > (CGI_TIMEOUT + 3))
                {
                    kill(pid, SIGKILL);
                    waitpid(pid, &status, 0);
                    state = ERROR;
                }
            }
        }
    }
}

void Cgi::handleCGI(Client &client)
{
    if (this->state == CHECKING)
        this->checkForCgi(client);
    if (this->state == SETUP_CGI)
        this->setupCgi(client);
    if (this->state == CREAT_PIPES)
        this->createPipes();
    if (this->state == EXECUTING)
        this->execution(client);
}

int Cgi::getPipeOutFd() const
{
    return pipeOut[0];
}

int Cgi::getPipeInFd() const
{
    return pipeIn[1];
}