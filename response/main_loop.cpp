#include "response.hpp"

void run(std::vector<Server> servers, epol *ep)
{
    std::map<int, Request> requests;
    std::map<int, clock_t> timer;
    std::map<int, std::string> stringOfrequest;
    while (1)
    {
        int fd_ready = epoll_wait(ep->ep_fd, ep->events, 1024, -1);
        if (fd_ready == -1)
            perror("epoll_wait");
        for (int i = 0; i < fd_ready; i++)
        {
            for (int j = 0; j < (int)servers.size(); j++)
            {
                if (servers[j].server_sock == ep->events[i].data.fd )
                {
                    accepting_new_clients(i,servers[j],ep,timer);
                }
            }
            for (int j = 0; j < (int)servers.size(); j++)
            {
                if (std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd) != servers[j].fd_sock.end())
                {
                    if(ep->events[i].events & EPOLLERR || ep->events[i].events & EPOLLHUP || ep->events[i].events & EPOLLRDHUP)
                    {
                        if(requests[ep->events[i].data.fd].is_forked_before == 1 && requests[ep->events[i].data.fd].child_exited == 0)
                        {
                            kill(requests[ep->events[i].data.fd].pid_of_the_child,SIGKILL);
                            waitpid(requests[ep->events[i].data.fd].pid_of_the_child, 0, 0);
                            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, requests[ep->events[i].data.fd].pipefd[0], NULL);
                            close(requests[ep->events[i].data.fd].pipefd[0]);
                        }
                        epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, ep->events[i].data.fd, NULL);
                        close(ep->events[i].data.fd);
                        std::map<int, Request>::iterator it = requests.find(ep->events[i].data.fd);
                        if (it != requests.end())
                            requests.erase(it);
                        servers[j].fd_sock.erase(std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd));
                    }
                    else if (ep->events[i].events & EPOLLIN && (requests.count(ep->events[i].data.fd) == 0 || requests[ep->events[i].data.fd].epol == 1))
                    {
                        request_part(servers, ep, ep->events[i].data.fd, requests, stringOfrequest);
                        requests[ep->events[i].data.fd].time_out = clock();
                    }
                    else if (ep->events[i].events & EPOLLOUT && requests.count(ep->events[i].data.fd) != 0 && requests[ep->events[i].data.fd].epol == 0 )
                    {
                        requests[ep->events[i].data.fd].time_out = clock();
                        if (!response(ep, ep->events[i].data.fd, requests,fd_ready))
                            end_reponse(ep,ep->events[i].data.fd,requests,servers[j]);
                    }
                    else if(requests.count(ep->events[i].data.fd) == 0)
                        check_timeout_ofnoreq(ep,timer,ep->events[i].data.fd,servers[j]);
                    else if(requests.count(ep->events[i].data.fd) != 0)
                    {
                       clock_t end = clock();
                        float elapsed_seconds = static_cast<float>(end - requests[ep->events[i].data.fd].time_out ) / CLOCKS_PER_SEC;
                        if(elapsed_seconds >= 10)
                        {  
                            epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, ep->events[i].data.fd, NULL);
                            close(ep->events[i].data.fd);
                            servers[j].fd_sock.erase(std::find(servers[j].fd_sock.begin(), servers[j].fd_sock.end(), ep->events[i].data.fd));
                            std::map<int, Request>::iterator it = requests.find(ep->events[i].data.fd);
                            if (it != requests.end())
                                requests.erase(it);
                        } 
                    }
                }

            }

        }
    }
}