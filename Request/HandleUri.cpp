#include "Request.hpp"

std::string &Request::replace_slash_in_target(Server &serv)
{
    int no_root_location = 0;

    for (int i = 0; i < (int)serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME == "/")
        {
            flag_uri = 1;
            no_root_location = 1;
            state_of_cgi = serv.locations[i].cgi;
            state_of_upload = serv.locations[i].upload_s;
            if ((method == "GET" && serv.locations[i].GET == false) || (method == "POST" && serv.locations[i].POST == false) || (method == "DELETE" && serv.locations[i].DELETE == false))
                status = "405";
            else if (method == "POST" && this->state_of_upload == 0)
            {
                if (!is_cgi)
                    status = "403";
            }
            if (serv.locations[i].upload_path == "")
            {
                if (serv.upload_path != "")
                    this->path_to_upload = serv.upload_path;
            }
            else
                this->path_to_upload = serv.locations[i].upload_path;
            if (serv.locations[i]._return == "")
            {
                if (serv.locations[i].index == "")
                {
                    if (serv.index == "")
                    {
                        if (serv.locations[i].autoindex == true)
                        {
                            if (serv.locations[i].root != "")
                                target = serv.locations[i].root;
                            else
                                target = serv.root;
                        }
                        else
                            status = "403";
                    }
                    else
                    { 
                        this->check_return_index = 1;
                        target = serv.index;
                    }
                        
                }
                else
                {
                    this->check_return_index = 1;
                    target = serv.locations[i].index;
                }
            }
            else
            {
                this->check_return_index = 1;
                target = serv.locations[i]._return;
                status = "301";
                this->uri_for_response = target;
                return target;
            }
        }
    }
    if (!no_root_location)
        target = serv.root;
    return (target);
}

int Request::count_slash(std::string tar)
{
    int count = 0;

    for (int i = 0; i < (int)tar.length(); i++)
        if (tar[i] == '/')
            count++;
    return (count);
}

void Request::short_uri(Server &serv)
{
    for (int i = 0; i < (int)serv.locations.size(); i++)
    {
        if (serv.locations[i].NAME != "/")
        {
            if (target == serv.locations[i].NAME || target.substr(1) == serv.locations[i].NAME)
            {
                flag_uri = 1;
                state_of_cgi = serv.locations[i].cgi;
                state_of_upload = serv.locations[i].upload_s;
                if ((method == "GET" && serv.locations[i].GET == false) || (method == "POST" && serv.locations[i].POST == false) || (method == "DELETE" && serv.locations[i].DELETE == false))
                    status = "405";
                else if (method == "POST" && this->state_of_upload == 0)
                {
                    if (!is_cgi)
                        status = "403";
                }
                if (serv.locations[i].upload_path == "")
                {
                    if (serv.upload_path != "")
                        this->path_to_upload = serv.upload_path;
                }
                else
                    this->path_to_upload = serv.locations[i].upload_path;
                if (serv.locations[i]._return == "")
                {
                    if (serv.locations[i].index == "")
                    {
                        if (serv.index == "")
                        {
                            if (serv.locations[i].autoindex == true)
                            {
                                if (serv.locations[i].root != "")
                                    target = serv.locations[i].root;
                                else
                                    target = serv.root;
                            }
                            else
                                status = "403";
                        }
                        else
                            target = serv.index;
                    }
                    else
                        target = serv.locations[i].index;
                }
                else
                {
                    target = serv.locations[i]._return;
                    this->uri_for_response = target;
                    status = "301";
                    return;
                }
            }
        }
    }
    if (!flag_uri)
    {
        std::string tmp(target);
        std::string path = replace_slash_in_target(serv);
        if (check_return_index)
            target = path;
        else
        {
            if (tmp[0] == '/')
                tmp = tmp.substr(0);
            target = path + tmp;
        }
    }
}

void Request::long_uri(Server &serv)
{
    std::vector<std::string> uri;
    int slash = 0;

    if (target[0] == '/')
        slash = 1;
    ft_split(target, "/", uri);
    target = "";
    int flag2;
    for (int j = 0; j < (int)uri.size(); j++)
    {
        flag2 = 0;
        for (int i = 0; i < (int)serv.locations.size(); i++)
        {
            if (serv.locations[i].NAME != "/")
            {
                if (uri[j] == serv.locations[i].NAME)
                {
                    slash = 0;
                    if (state_of_cgi != 0)
                        state_of_cgi = serv.locations[i].cgi;
                    if (state_of_upload != 0)
                        state_of_upload = serv.locations[i].upload_s;
                    if ((method == "GET" && serv.locations[i].GET == false) || (method == "POST" && serv.locations[i].POST == false) || (method == "DELETE" && serv.locations[i].DELETE == false))
                        status = "405";
                    else if (method == "POST" && this->state_of_upload == 0)
                    {
                        if (!is_cgi)
                            status = "403";
                    }
                    if (serv.locations[i].upload_path == "")
                    {
                        if (serv.upload_path != "")
                            this->path_to_upload = serv.upload_path;
                    }
                    else
                        this->path_to_upload = serv.locations[i].upload_path;

                    if (serv.locations[i]._return == "")
                    {
                        if (serv.locations[i].index == "")
                        {
                            if (serv.index == "")
                            {
                                if (serv.locations[i].autoindex == true)
                                {
                                    if (serv.locations[i].root != "")
                                        target = serv.locations[i].root;
                                    else
                                        target = serv.root;
                                }
                                else
                                    status = "403";
                            }
                            else
                                target = serv.index;
                        }
                        else
                        {
                            target = serv.locations[i].index;
                            return;
                        }
                    }
                    else
                    {
                      
                        target = serv.locations[i]._return;
                        this->uri_for_response = target;
                        status = "301";
                        return;
                    }
                    flag_uri = 1;
                    flag2 = 1;
                    if (status != "200")
                        return;
                }
            }
        }
        if (!flag2)
            target += uri[j];
        if (j < (int)uri.size() - 1)
        {
            if (target != "")
            {
                if (target[target.length() - 1] != '/')
                    target += "/";
            }
        }
    }
    if (slash)
        target = "/" + target;
    if (!flag_uri)
    {
        std::string tmp(target);
        std::string path = replace_slash_in_target(serv);
        if (check_return_index)
            target = path;
        else
        {
            if (tmp[0] == '/')
                tmp = tmp.substr(0);
            target = path + tmp;
        }
    }
}

void Request::directory_moved_permanently()
{
    struct stat fileStat;
    if (stat(target.c_str(), &fileStat) == 0)
    {
        if (S_ISDIR(fileStat.st_mode))
        {
            if (uri_for_response[uri_for_response.length() - 1] != '/')
            {
                
                status = "301";
                this->uri_for_response += "/";
            }
        }
    }
}

void Request::encoded_uri()
{
    std::string newUri;
    std::map<std::string, char> myMap;

    myMap.insert(std::pair<std::string, char>("20", ' '));
    myMap.insert(std::pair<std::string, char>("3C", '<'));
    myMap.insert(std::pair<std::string, char>("3E", '>'));
    myMap.insert(std::pair<std::string, char>("23", '#'));
    myMap.insert(std::pair<std::string, char>("25", '%'));
    myMap.insert(std::pair<std::string, char>("3F", '?'));
    myMap.insert(std::pair<std::string, char>("26", '&'));
    myMap.insert(std::pair<std::string, char>("2F", '/'));
    myMap.insert(std::pair<std::string, char>("5C", '\\'));
    myMap.insert(std::pair<std::string, char>("3A", ':'));
    myMap.insert(std::pair<std::string, char>("3B", ';'));
    myMap.insert(std::pair<std::string, char>("3D", '='));
    myMap.insert(std::pair<std::string, char>("2B", '+'));

    for (int i = 0; i < (int)target.length(); i++)
    {
        if (target[i] == '%')
        {
            std::string value(target.substr(i + 1, 2));
            newUri += myMap[value];
            i += 2;
        }
        else
            newUri += target[i];
    }
    target = newUri;
}