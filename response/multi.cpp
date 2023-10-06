#include "response.hpp"

uint32_t ipToUint32(std::string &ip)
{
    std::vector<uint32_t> octets;
    std::istringstream iss(ip);
    std::string octet;
    while (std::getline(iss, octet, '.'))
    {
        octets.push_back(atoi(octet.c_str()));
    }
    return (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
}

void fill_ser_Add(Server &ser)
{
    ser.seraddr_s.sin_family = AF_INET;
    ser.seraddr_s.sin_addr.s_addr = htonl(ipToUint32(ser.host));
    ser.seraddr_s.sin_port = htons(ser.listen);
}

void init(Server &ser, epol *ep)
{
        if ((ser.server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("failed to create socket");
            exit(1);
        }
        int reuse = 1;
        if (setsockopt(ser.server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        {
            perror("setsockopt SO_REUSEADDR failed");
            exit(1);
        }
        fill_ser_Add(ser);
        if ((bind(ser.server_sock, (struct sockaddr *)&ser.seraddr_s, sizeof(ser.seraddr_s))) < 0)
        {
            perror("bindddddd");
            exit(1);
        }
        if ((listen(ser.server_sock, 10)) < 0)
        {
            perror("listen");
            exit(1);
        }
        ep->ev.events = EPOLLIN;
        ep->ev.data.fd = ser.server_sock;
        if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, ser.server_sock, &ep->ev) == -1)
        {
            close(ep->ep_fd);
            close(ser.server_sock);
            perror("epoll ctl");
            exit(1);
        }
}

void accepting_new_clients(int i,Server& servers,epol *ep,std::map<int,clock_t> &timer)
{
   
        int client_fd = accept(ep->events[i].data.fd, NULL, NULL);

        if (client_fd == -1)
        {
            perror("accept fun");
            return;
        }
        timer[client_fd]= clock();
        ep->ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
        ep->ev.data.fd = client_fd;
        servers.fd_sock.push_back(client_fd);
        if (epoll_ctl(ep->ep_fd, EPOLL_CTL_ADD, client_fd, &ep->ev) == -1)
        {
            perror("epol_ctl");
            close(client_fd);
        }
}

void end_reponse(epol *ep, int client_fd, std::map<int, Request> &req,Server& servers)
{
    closing( client_fd, req);
    epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, client_fd, NULL);
    close(client_fd);
    std::map<int, Request>::iterator it = req.find(client_fd);
    if (it != req.end())
        req.erase(it);
    servers.fd_sock.erase(std::find(servers.fd_sock.begin(), servers.fd_sock.end(), client_fd));
}
void check_timeout_ofnoreq(epol *ep,std::map<int, clock_t>& timer,int fd,Server& servers)
{
    clock_t end = clock();
    float elapsed_seconds = static_cast<float>(end - timer[fd] ) / CLOCKS_PER_SEC;
    if(elapsed_seconds >= 10)
    {
        
        epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        servers.fd_sock.erase(std::find(servers.fd_sock.begin(), servers.fd_sock.end(), fd));
        timer.erase(fd);
    }
}