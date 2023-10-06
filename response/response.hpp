#pragma once

#include "../Request/Request.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#define MAX_REQUEST_SIZE 2048
#define LINE 4096

struct cgi_args
{
    char *args[10];
    char *env[10];
};
class Response
{
    private : 
        int client_fd;
        std::map<int, Request> &req;
    public:
        Response(int f,std::map<int, Request> &r);
  
        int get_client_fd();
        
        bool is_get();
       
        bool is_cgi();
        
        bool is_delete();
       
        bool is_error();
        
        bool is_redirection();
        
        bool is_post();
        bool is_cookie_sit();
      
        std::string normal_pages_header(size_t contentLength);
       
        std::string redirect_pages_header();
        
        void response_by_a_page(std::string path);
       
        std::string get_content_type();
       
        int directorie_list();
        
        std::string chunked_header();
       
        int chunked_response_headers();
        int get_request();
        int chunked_response_body();
        std::string cgi_header(std::string all_headers);
        std::string generateDirectoryListing();
        std::string get_last();
        std::string cookie_header();
};

std::string valueOfkey(std::string key, std::vector<std::pair<std::string, std::string> > &postR);

typedef struct ep
{
    int  ep_fd;
    struct epoll_event ev;
    struct epoll_event events[1024];
} epol ;

void init(Server& ser,epol *ep);
void fill_ser_Add(Server &ser,int i);
void run( std::vector<Server> servers,epol *ep);
void fill_ser_Add(Server &ser,int i);
int response(epol *ep, int client_fd, std::map<int, Request> &req,int fd_ready);

std::string removeSpaces(const std::string &input);
location *get_location(std::ifstream &Myfile, std::string &line);
std::vector<Server> mainf(int ac, char **av);
std::string constructResponseHeader(const std::string &contentType, std::string status);
std::string birng_content(std::vector<Request> req, int reciver);
std::string opendir(const std::string &directoryPath, std::map<int, Request> &req, int client_fd);
std::string get_content_type(const char *path);
int checking_timeout(int client_fd, std::map<int, Request> &req,Response& resp);
void check_timeout_ofnoreq(epol *ep,std::map<int, clock_t>& timer,int fd,Server& servers);
class args : std::exception{
    public:
    const char *what() const throw()
    {
        return "configue file needed";
    }
};
void fill_envirements(cgi_args *cgi, std::map<int, Request> &req, int client_fd);
void forking( int client_fd, std::map<int, Request> &req);
void child_proc(int client_fd, std::map<int, Request> &req);
int adding_pipe_2ep(epol *ep, int client_fd, std::map<int, Request> &req);
int cgi_response(epol *ep, int client_fd, std::map<int, Request> &req,Response& resp);
void accepting_new_clients(int i,Server& servers,epol *ep,std::map<int,clock_t> &timer);
void request_part(std::vector<Server> &servers, epol *ep, int client_fd, std::map<int, Request> &req, std::map<int, std::string> &stringOfrequest);
void end_reponse(epol *ep, int client_fd, std::map<int, Request> &req,Server& servers);
void closing(int client_fd, std::map<int, Request> &req);
