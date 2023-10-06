#include "response.hpp"

#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>
int Request::num_file = 0;

std::string get_current_time()
{
    std::time_t currentTime;
    std::time(&currentTime);
    char timeString[100];
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S", std::localtime(&currentTime));
    std::string time(timeString);

    return (time);
}

size_t get_chunk_size(std::string str)
{
    size_t pos;
    pos = str.find("\r\n");
    char *status;
    size_t chunk_size = std::strtol(str.substr(0, pos).c_str(), &status, 16);

    return chunk_size;
}

void remove_size_of_chunk(std::string &str)
{
    str = str.substr(str.find("\n") + 1);
}

std::string get_file_extension(std::string &str, int client_fd, std::map<int, Request> &req)
{
    std::string ext;
    size_t pos;
    try
    {
        pos = str.find("Content-Disposition: ");
        str = str.substr(pos);
        pos = str.find("\r\n");
        str = str.substr(pos + 2);

        std::string tmp;
        pos = str.find("\n");
        tmp = str.substr(0, pos);
        if (tmp.find("Content-Type: ") != tmp.npos)
        {
            pos = str.find("Content-Type: ");
            if (pos != str.npos)
            {
                str = str.substr(pos);
                pos = str.find("\n");
                ext = str.substr(14, pos - 15);
                pos = str.find("\r\n\r\n");
                str = str.substr(pos + 4);
                return (req[client_fd].extensions[ext]);
            }
        }
        str = str.substr(2);
    }
    catch (...)
    {
    }
    return (".txt");
}

void write_content(int client_fd, std::map<int, Request> &req, int close, std::string &content, int size)
{
    req[client_fd].outfile.write(content.c_str(), size);
    req[client_fd].outfile.flush();
    if (close)
        req[client_fd].outfile.close();
}

std::string generate_file_name(std::string &str, int client_fd, std::map<int, Request> &req)
{
    std::string time_tmp;
    std::stringstream s;
    s << Request::num_file++;
    time_tmp += s.str();
    std::string pt;

    pt = time_tmp + get_file_extension(str, client_fd, req);
    if (req[client_fd].target.find(".php") != req[client_fd].target.npos || req[client_fd].target.find(".py") != req[client_fd].target.npos)
        pt += ".txt";
    return pt;
}

void open_file(std::string &str, int client_fd, std::map<int, Request> &req)
{
    req[client_fd].outfile_name = req[client_fd].path_to_upload + generate_file_name(str, client_fd, req);
    req[client_fd].outfile.open(req[client_fd].outfile_name.c_str(), std::ios::binary);
    req[client_fd].open_boundry_file = 1;
}

void multiple_file_in_chunk(std::string &str, int client_fd, std::map<int, Request> &req, size_t pos)
{
    while (1)
    {
        if (req[client_fd].open_boundry_file == 0)
        {
            open_file(str, client_fd, req);
        }

        else if ((pos = str.find(req[client_fd].boundary_separater)) != str.npos)
        {
            std::string s = str.substr(pos);
            if (strncmp(s.c_str(), (req[client_fd].boundary_separater + "--").c_str(), req[client_fd].boundary_separater.length() + 2) == 0)
            {
                write_content(client_fd, req, 1, str, pos);
                req[client_fd].epol = 0;
                return;
            }
            else
            {
                write_content(client_fd, req, 1, str, pos);
                req[client_fd].open_boundry_file = 0;
            }
        }
    }
}

void Boundry(int client_fd, std::map<int, Request> &req, std::string &str)
{
    size_t pos;
    size_t pos1;

    if (req[client_fd].open_boundry_file == 0)
        open_file(str, client_fd, req);
    if ((str.find(req[client_fd].boundary_separater)) == str.npos && (str.find(req[client_fd].boundary_separater + "--")) == str.npos)
    {
        int rest;
        if (str.length() > req[client_fd].boundary_separater.length())
            rest = str.length() - req[client_fd].boundary_separater.length();
        else
            rest = str.length();

        write_content(client_fd, req, 0, str, rest);
        req[client_fd].rest_of_boundry = str.substr(rest);
    }
    else if ((pos = str.find(req[client_fd].boundary_separater)) != str.npos && (pos1 = str.find(req[client_fd].boundary_separater + "--")) != str.npos && pos != pos1)
    {
        write_content(client_fd, req, 1, str, pos);
        req[client_fd].open_boundry_file = 0;
        multiple_file_in_chunk(str, client_fd, req, pos);
    }
    else if ((pos = str.find(req[client_fd].boundary_separater + "--")) != str.npos) // last separater
    {
        int rest = str.length() - (str.length() - pos);
        write_content(client_fd, req, 1, str, rest);
        req[client_fd].epol = 0;
        return;
    }
    else if ((pos = str.find(req[client_fd].boundary_separater)) != str.npos) // midle separater
    {
        int rest = str.length() - (str.length() - pos);
        write_content(client_fd, req, 1, str, rest);

        req[client_fd].rest_of_boundry = str.substr(pos + 1);
        req[client_fd].open_boundry_file = 0;
    }
}

int chunked_content_lenght(int client_fd, std::map<int, Request> &req)
{
    if (req[client_fd].maxbody > 0)
    {
        req[client_fd].max_readed += req[client_fd].chunk_size;
        if (req[client_fd].max_readed > req[client_fd].maxbody)
        {
            req[client_fd].outfile.close();
            req[client_fd].status = "";
            req[client_fd].target = "error/413.html";
            std::remove(req[client_fd].outfile_name.c_str());
            req[client_fd].epol = 0;
            return 1;
        }
    }
    return 0;
}

void Chunked(int client_fd, std::map<int, Request> &req, std::string &str, int n_read)
{
    if (req[client_fd].calcul_chunk_flag == 0)
    {
        req[client_fd].chunk_size = get_chunk_size(str);
        if (req[client_fd].chunk_size == 0)
        {
            req[client_fd].outfile.close();
            req[client_fd].epol = 0;
            return;
        }
        remove_size_of_chunk(str);
        req[client_fd].calcul_chunk_flag = 1;
    }
    if (req[client_fd].chunk_size - ((int)str.size() - 2) < 0)
    {
        req[client_fd].outfile.write(str.c_str(), req[client_fd].chunk_size);
        req[client_fd].outfile.close();
        req[client_fd].epol = 0;
        return;
    }
    req[client_fd].chunk_size -= str.size();
    if (n_read == req[client_fd].Bytes_readed - 1 && req[client_fd].Bytes_readed < 1024)
        req[client_fd].outfile.write(str.c_str(), str.size() - 2);
    else
        req[client_fd].outfile.write(str.c_str(), str.size());
    req[client_fd].outfile.flush();
    if (n_read < req[client_fd].Bytes_readed - 1)
        req[client_fd].Bytes_readed -= n_read;
    else if (req[client_fd].Bytes_readed < 1024)
    {
        req[client_fd].Bytes_readed = 1024;
        req[client_fd].calcul_chunk_flag = 0;
    }
    else if (req[client_fd].chunk_size < req[client_fd].Bytes_readed)
        req[client_fd].Bytes_readed = req[client_fd].chunk_size + 3;
}

void Chunked_helper(int client_fd, std::map<int, Request> &req, std::string &str)
{
    if (req[client_fd].calcul_chunk_flag == 0)
    {
        req[client_fd].chunk_size = get_chunk_size(str);

        if (req[client_fd].chunk_size == 0)
        {
            req[client_fd].outfile.close();
            req[client_fd].epol = 0;
            return;
        }
        remove_size_of_chunk(str);
        req[client_fd].calcul_chunk_flag = 1;
    }
    if (req[client_fd].chunk_size - ((int)str.size() - 2) <= 0)
    {
        req[client_fd].outfile.write(str.c_str(), req[client_fd].chunk_size);
        req[client_fd].outfile.close();
        req[client_fd].epol = 0;
        return;
    }
    req[client_fd].chunk_size -= str.size();
    req[client_fd].outfile.write(str.c_str(), str.size());
}

void droping(std::vector<Server> &servers, epol *ep, int client_fd)
{
    epoll_ctl(ep->ep_fd, EPOLL_CTL_DEL, client_fd, NULL);
    close(client_fd);
    for (int j = 0; j < (int)servers.size(); j++)
    {
        for (int i = 0; i < (int)servers[j].fd_sock.size(); i++)
        {
            if (servers[j].fd_sock[i] == client_fd)
            {
                servers[j].fd_sock.erase(std::find(servers[i].fd_sock.begin(), servers[i].fd_sock.end(), client_fd));
                return;
            }
        }
    }
}
void closing(int client_fd, std::map<int, Request> &req)
{
    req[client_fd].outfile.close();
    req[client_fd].fd_file.close();
}

void read_headers(std::vector<Server> &servers, epol *ep, int client_fd, std::map<int, Request> &req, std::map<int, std::string> &stringOfrequest)
{
    int n_read;
    char rec[1024];

    memset(rec, 0, 1023);
    if ((n_read = read(client_fd, rec, 1023)) > 0)
        stringOfrequest[client_fd].append(rec, n_read);
    else if (n_read <= 0)
    {
        perror("read0 : ");
        droping(servers, ep, client_fd);
        return;
    }
    if (stringOfrequest[client_fd].find("\r\n\r\n") != std::string::npos || n_read < 1023)
    {
        int flag = 0;
        for (int j = 0; j < (int)servers.size(); j++)
        {
            for (int i = 0; i < (int)servers[j].fd_sock.size(); i++)
            {
                if (servers[j].fd_sock[i] == client_fd)
                {
                    Request obj(stringOfrequest[client_fd], servers[j], servers);
                    req.insert(std::pair<int, Request>(client_fd, obj));
                    flag = 1;
                    break;
                }
                if (flag == 1)
                    break;
            }
        }
        stringOfrequest.erase(client_fd);
        if (req[client_fd].status == "200" && req[client_fd].method == "POST")
        {
            if (req[client_fd].Post_status == "boundary")
            {
                if (req[client_fd].Body.find(req[client_fd].boundary_separater + "--") != std::string::npos)
                {
                    req[client_fd].Body.clear();
                    Boundry(client_fd, req, req[client_fd].Body);
                }
            }

            else if (req[client_fd].Post_status == "Bainary/Row")
            {
                req[client_fd].lenght_Readed += req[client_fd].Body.size();

                if (!req[client_fd].Body.empty())
                {
                    req[client_fd].outfile.write(req[client_fd].Body.c_str(), req[client_fd].Body.size());
                    req[client_fd].Body.clear();
                }
                if (req[client_fd].lenght_Readed == req[client_fd].lenght_of_content)
                {
                    req[client_fd].Body.clear();
                    req[client_fd].outfile.close();
                    req[client_fd].epol = 0;
                    return;
                }
            }

            else if (req[client_fd].Post_status == "chunked")
            {
                if (!req[client_fd].Body.empty())
                {
                    Chunked_helper(client_fd, req, req[client_fd].Body);
                    req[client_fd].Body.clear();
                }
            }
        }

        if (req[client_fd].endOfrequest || req[client_fd].status != "200")
            req[client_fd].epol = 0;
    }
}

void request_part(std::vector<Server> &servers, epol *ep, int client_fd, std::map<int, Request> &req, std::map<int, std::string> &stringOfrequest)
{
    if (req.count(client_fd) == 0)
        read_headers(servers, ep, client_fd, req, stringOfrequest);
    else
    {
        int n_read;
        size_t size = req[client_fd].Bytes_readed;
        char rec_b[size];

        memset(rec_b, 0, size - 1);
        if ((n_read = read(client_fd, rec_b, size - 1)) > 0)
        {
            if (req[client_fd].Post_status == "Bainary/Row")
            {
                req[client_fd].lenght_Readed += n_read;
                req[client_fd].outfile.write(rec_b, n_read);
                if (req[client_fd].lenght_Readed >= req[client_fd].lenght_of_content)
                {
                    req[client_fd].outfile.close();
                    req[client_fd].epol = 0;
                    return;
                }
            }
            else if (req[client_fd].Post_status == "chunked")
            {
                std::string str;
                str.append(rec_b, n_read);
                Chunked(client_fd, req, str, n_read);
            }
            else if (req[client_fd].Post_status == "boundary")
            {
                if (!req[client_fd].Body.empty())
                {
                    std::string str;
                    str = req[client_fd].Body;
                    req[client_fd].Body.clear();
                    Boundry(client_fd, req, str);
                }
                if (n_read  > 0)
                {
                    std::string str;
                    str.append(req[client_fd].rest_of_boundry);
                    str.append(rec_b, n_read);
                    Boundry(client_fd, req, str);
                }
            }
            memset(rec_b, 0, size - 1);
        }
        else if (n_read <= 0)
        {
            perror("read2 : ");
            droping(servers, ep, client_fd);
            closing(client_fd, req);
            std::map<int, Request>::iterator it = req.find(client_fd);
            if (it != req.end())
                req.erase(it);
        }
    }
}
