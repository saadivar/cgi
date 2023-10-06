
#include "response.hpp"



void fill_envirements(cgi_args *cgi, std::map<int, Request> &req, int client_fd)
{
    std::string script;
    std::string path;

    script = "SCRIPT_FILENAME=" + req[client_fd].target ;
    path = "PATH_INFO=" + req[client_fd].target ;

  
    if ((req[client_fd].target.find(".php")) != std::string::npos)
        cgi->args[0] = (char*)"/usr/bin/php-cgi";
    else
        cgi->args[0] = (char*)"/usr/bin/python3.10" ;
    cgi->args[1] = (char*)req[client_fd].target.c_str();
    cgi->args[2] = NULL;
    if (req[client_fd].method == "GET")
    {
        cgi->env[0] =(char *)"REQUEST_METHOD=GET" ;
        cgi->env[1] = (char *)req[client_fd].query.c_str();
        cgi->env[2] = (char*)"REDIRECT_STATUS=200";
        cgi->env[3] = strdup(script.c_str());
        cgi->env[4] = strdup(path.c_str());
        cgi->env[5] = (char *)req[client_fd].cookie.c_str();
        cgi->env[6] = (char *)req[client_fd].accept.c_str();
        cgi->env[7] = NULL;
    }
    else if (req[client_fd].method == "POST")
    {
        cgi->env[0] =(char *)"REQUEST_METHOD=POST" ;
        cgi->env[1] = (char *)req[client_fd].content_lenght.c_str();
        cgi->env[2] = (char *)req[client_fd].content_type.c_str();
        cgi->env[3] = (char*)"REDIRECT_STATUS=200";
        cgi->env[4] = strdup(script.c_str());
        cgi->env[5] = strdup(path.c_str());
        cgi->env[6] = (char *)req[client_fd].accept.c_str();
        cgi->env[7] = (char *)req[client_fd].cookie.c_str();
        cgi->env[8] = (char *)"PHPRC=directorie/php.ini";

        cgi->env[9] = NULL;
    }
}

void forking( int client_fd, std::map<int, Request> &req)
{
    req[client_fd].is_forked_before = 1;
    if (pipe(req[client_fd].pipefd) == -1)
        perror("pipe");
    req[client_fd].pid_of_the_child = fork();

    if (req[client_fd].pid_of_the_child  == -1)
        perror("fork");
}

void child_proc(int client_fd, std::map<int, Request> &req)
{
    cgi_args args;
    close(req[client_fd].pipefd[0]); 
    dup2(req[client_fd].pipefd[1], 1); 
    close(req[client_fd].pipefd[1]);
    int fd ;
    if(req[client_fd].method == "POST")
    {
        fd = open(req[client_fd].outfile_name.c_str(),O_RDONLY | std::ios::binary);
        dup2(fd, 0);
        close (fd);
    }   
    fill_envirements(&args, req, client_fd);
    if (execve(args.args[0], args.args, args.env) == -1)
    {
        perror("exec = ");
    }
    exit(127);
}

int adding_pipe_2ep(epol *ep, int client_fd, std::map<int, Request> &req)
{
    ep->ev.events = EPOLLIN ;
    ep->ev.data.fd = req[client_fd].pipefd[0];
    if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, req[client_fd].pipefd[0], &ep->ev) == -1)
    {
        epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, req[client_fd].pipefd[0], &ep->ev);
        close(req[client_fd].pipefd[0]);
        return 0;
    }
    req[client_fd].time_of_child = clock();
    close(req[client_fd].pipefd[1]); 
    return 1;
}

int checking_timeout(int client_fd, std::map<int, Request> &req,Response& resp)
{
     pid_t retwait = waitpid(req[client_fd].pid_of_the_child, 0, WNOHANG);
    if (retwait == 0)
    {
        clock_t end = clock();
        double elapsed_seconds = static_cast<double>(end - req[client_fd].time_of_child) / CLOCKS_PER_SEC;
        if(elapsed_seconds >= 10)
        {
            std::remove(req[client_fd].outfile_name.c_str());
            close(req[client_fd].pipefd[0]); 
            kill(req[client_fd].pid_of_the_child, SIGKILL);
            waitpid(req[client_fd].pid_of_the_child, 0, 0);
            req[client_fd].status = "504";
            resp.response_by_a_page("error/504.html");
            return 0;
        }
        else
            return 1;       
    }
    else
        req[client_fd].child_exited = 1;
    return 1;
}

int cgi_response(epol *ep, int client_fd, std::map<int, Request> &req,Response& resp)
{
    char buffer[70000];
    ssize_t bytesRead;
    if ((bytesRead = read(req[client_fd].pipefd[0], buffer, sizeof(buffer))) > 0)
    {        
        std::string header;
        std::string body;
        header.append(buffer,bytesRead);
        body.append(buffer,bytesRead);
        header = header.substr(0,header.find("\r\n\r\n")) ;
        body = body.substr(header.find("\r\n\r\n") + 4);
        std::string response_header = resp.cgi_header(header);
        response_header.append(buffer,bytesRead);
        if(write(client_fd,response_header.c_str(), response_header.size()) <= 0)
        {
            std::remove(req[client_fd].outfile_name.c_str());
            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, req[client_fd].pipefd[0], NULL);
            close(req[client_fd].pipefd[0]);
            return 0;
        }
        std::remove(req[client_fd].outfile_name.c_str());
        epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, req[client_fd].pipefd[0], NULL);
        close(req[client_fd].pipefd[0]);
    }
    else if(bytesRead <= 0)
        return 0;
    return 0;
}