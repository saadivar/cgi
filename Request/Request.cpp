#include "Request.hpp"

Request::Request(){
}

Request::Request(Request const &req)
{
    *this = req;
}

Request &Request::operator=(Request const &req)
{
    this->method = req.method;
    this->target = req.target;
    this->httpVersion = req.httpVersion;
    this->status = req.status;
    this->header_flag = req.header_flag;
    this->endOfrequest = req.endOfrequest;
    this->lenght_Readed = req.lenght_Readed;
    this->Post_status = req.Post_status;
    this->lenght_of_content = req.lenght_of_content;
    this->extension = req.extension;
    this->outfile_name = req.outfile_name;
    this->open_boundry_file = req.open_boundry_file;
    this->rest_of_boundry = req.rest_of_boundry;
    this->chunk_size = req.chunk_size;
    this->calcul_chunk_flag = req.calcul_chunk_flag;
    this->Bytes_readed = req.Bytes_readed;
    this->flag_uri = req.flag_uri;
    this->uri_for_response = req.uri_for_response;
    this->boundary_separater = req.boundary_separater;
    this->query = req.query;
    this->content_type = req.content_type;
    this->content_lenght = req.content_lenght;
    
    this->outfile.copyfmt(req.outfile);
    this->outfile.clear();
    this->outfile.open(req.outfile_name.c_str());
    this->fd_file.copyfmt(req.fd_file);
    this->fd_file.clear();
    this->fd_file.open(req.target.c_str());
    
    this->chunked_file_size_response = req.chunked_file_size_response;
    this->content_type_python = req.content_type_python;
    this->epol = req.epol;
    this->is_forked_before = req.is_forked_before;
    this->is_forked_before =req.is_forked_before;
    this->pid_of_the_child = req.pid_of_the_child;
    this->time_of_child = req.time_of_child;
    this->header_for_cgi_resp =req.header_for_cgi_resp;
    this->child_exited =req.child_exited;
    this->pipefd[0] = req.pipefd[0];
    this->pipefd[1] = req.pipefd[1];
    this->cookie = req.cookie;
    this->path_to_upload = req.path_to_upload;
    this->extensions = req.extensions;
    this->state_of_cgi = req.state_of_cgi;
    this->state_of_upload = req.state_of_upload;
    this->Body = req.Body;
    this->is_cgi = req.is_cgi;
    this->maxbody = req.maxbody;
    this->max_readed = req.max_readed;
    this->last_chunk = req.last_chunk;
    this->time_out = req.time_out;
    return (*this);
}

void Request::init()
{
    this->status = "200";
    this->header_flag = 0;
    this->chunked_file_size_response = 0;
    this->endOfrequest = 1;
    this->lenght_Readed = 0;
    this->calcul_chunk_flag = 0;
    this->Bytes_readed = 1024;
    this->open_boundry_file = 0;
    this->chunk_size = 0;
    this->flag_uri = 0;
    this->epol = 1;
    this->is_forked_before = 0;
    this->pid_of_the_child = -1;
    this->time_of_child = -1;
    this->header_for_cgi_resp = 0;
    this->child_exited = 0;
    this->pipefd[0] = -1;
    this->path_to_upload = "/upload";
    this->state_of_cgi = 1;
    this->state_of_upload = 1;
    this->Body = "";
    this->is_cgi = 0;
    this->lenght_of_content = 0;
    this->maxbody = -1;
    this->max_readed = 0;
    this->last_chunk = 0;
    this->time_out = 0;
    this->check_return_index = 0;
}

void Request::search_for_ServerName(std::vector<Server> &servers, Server &serv)
{
    std::vector<std::string> tmp;

    ft_split(valueOfkey("Host", StoreHeaders), ":", tmp);
    for (int i = 0; i < (int)servers.size(); i++)
    {
        if (servers[i].server_name == tmp[0])
        {
            serv = servers[i];
            break;
        } 
    }
}

void Request::real_path()
{
    char new_path[PATH_MAX];

    if (realpath(target.c_str(), new_path) != 0)
    {
        std::string tmp(new_path);
        
        if(tmp.find("directorie") == tmp.npos)
            status = "403";
    }
}

Request::Request(std::string req, Server server, std::vector<Server> &servers)
{
    init();
    size_t pos = req.find("\r\n\r\n");
    if (pos != req.npos)
    {
        Body = req.substr(pos + 4);
        req = req.substr(0, pos + 2);
    }
 
    ft_split(req, "\r\n", myHeaders);
    fill_method_type();
    fill_query();
    fill_error_pages_map();
    fill_extensions_map();
    this->uri_for_response = target;
    fill_headers();
 
    if (status == "200" && (target.find(".php") != target.npos || target.find(".py") != target.npos))
        is_cgi = 1;
    if (status == "200" && find_key("Host", StoreHeaders))
        search_for_ServerName(servers, server);
    
    if (status == "200")
        error_handling(server);

    if (method == "POST" && status == "200")
    {
        this->endOfrequest = 0;  
        get_post_status();
    }

    if (method == "DELETE" && status == "200")
    {
        Delete_methode();
        if (status == "200")
            target = "error/200.html";
    }
    if (find_key("Content-Length", StoreHeaders) || find_key("Content-Type", StoreHeaders))
    {
        content_type = "CONTENT_TYPE=" + valueOfkey("Content-Type", StoreHeaders);
        content_lenght = "CONTENT_LENGTH=" + valueOfkey("Content-Length", StoreHeaders);
    }
    this->accept = valueOfkey("Accept", StoreHeaders);
    this->cookie = "HTTP_COOKIE=";
    if (find_key("Cookie", StoreHeaders))
        this->cookie += valueOfkey("Cookie", StoreHeaders);
    generate_error_page(server);
} 

Request::~Request(){
} 