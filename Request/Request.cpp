#include "Request.hpp"

Request::Request()
{
    this->header_flag = 0;
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
   
    query = req.query;
 
 
    content_type = req.content_type;
    content_lenght = req.content_lenght;

    this->outfile.copyfmt(req.outfile);
    this->outfile.clear();
    outfile.open(req.outfile_name.c_str());


    this->fd_file.copyfmt(req.fd_file);
    this->fd_file.clear();
    fd_file.open(req.target.c_str());

    this->chunked_file_size_response = req.chunked_file_size_response;


    content_type_python = req.content_type_python;
    this->epol = req.epol;
    this->is_forked_before = req.is_forked_before;
    this->is_forked_before =req.is_forked_before;
    this->pid_of_the_child = req.pid_of_the_child;
    this->time_of_child = req.time_of_child;
    this->header_for_cgi_resp =req.header_for_cgi_resp;
    this->child_exited =req.child_exited;
    this->pipefd[0] = req.pipefd[0];
    this->pipefd[1] = req.pipefd[1];

    return (*this);
}

Request::Request(std::string req, Server server)
{
    int filmap = 0;
    this->status = "200";
    this->header_flag = 0;
    this->chunked_file_size_response = 0;
    this->endOfrequest = 1;
    this->lenght_Readed = 0;
    this->calcul_chunk_flag = 0;
    this->Bytes_readed = 1024;
    this->open_boundry_file = 0;
    this->chunk_size = 0;
    this->rest_of_boundry = "";
    this->flag_uri = 0;
    this->content_type = "";
    this->epol = 1;
    this->content_lenght = "";
    this->extension = "";
    is_forked_before = 0;
    pid_of_the_child = -1;
    time_of_child = -1;
    header_for_cgi_resp = 0;
    child_exited = 0;
    ft_split(req, "\r\n", myHeaders);
    fill_method_type();
    //std::cout << target << std::endl;
    fill_query();
    this->uri_for_response = target;
    fill_headers();
    
   //print Headers
    //for (int i = 0; i < StoreHeaders.size(); i++)
     // std::cout << "val = " << StoreHeaders[i].first << " key = " << StoreHeaders[i].second << std::endl;
    
    error_handling(server); 
    //std::cout <<status<<std::endl;
    //std::cout <<target<<std::endl;
    if (method == "POST" && status == "200")
    {
        this->endOfrequest = 0;
        
        get_post_status();
    }
    directory_moved_permanently();

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


    fill_error_pages_map();
    //default error page
    generate_error_page(server);
   // std::cout << "final "<< target << std::endl;

} 

Request::~Request()
{
}