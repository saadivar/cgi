#include "server.hpp"

location::location()
{
    NAME = "";
    root = "";    
    index = "";
    _return = "";
    upload_path = "";
    POST = 0;
    autoindex = 1;
    GET = 1;
    DELETE = 0;
    cgi = 0;
    upload_s = 0;
}

int slash_num(std::string tar)
{
    int count = 0;
    
    for (int i = 0; i < (int)tar.length(); i++)
        if (tar[i] == '/')
            count++;
    return (count);
}

int location::post(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.POST = true;
    else if (hold[1] == "off")
        location.POST = false;
    else
        return 0;
    duplicate_in_location.push_back("POST");
    return 1;
}

int location::get(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.GET = true;
    else if (hold[1] == "off")
        location.GET = false;
    else
        return 0;
    duplicate_in_location.push_back("GET");

    return 1;
}

int location::auto_index(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.autoindex = true;
    else if (hold[1] == "off")
        location.autoindex = false;
    else
        return 0;
    duplicate_in_location.push_back("autoindex");
    return 1;
}

int location::deletee(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.DELETE = true;
    else if (hold[1] == "off")
        location.DELETE = false;
    else
        return 0;
    duplicate_in_location.push_back("delete");
    
    return 1;
}

int location::name(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (slash_num(hold[1]) > 1)
       return 0;
    if (hold[1] != "/" && hold[1][hold[1].length() - 1] == '/')
        return 0;
    if (hold[1] != "/" && hold[1][0] == '/')
        hold[1] = hold[1].substr(1);

    location.NAME = hold[1];
    return 1;
}

int location::rreturn(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    duplicate_in_location.push_back("return");
    location._return = hold[1];
    return 1;
}

int location::root_name(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (access(hold[1].c_str(), F_OK) == -1)
        return 0;
    if (hold[1][hold[1].length() - 1] != '/')
        return (0);
    location.root = hold[1];
    duplicate_in_location.push_back("root");
    return 1;
}

int location::loc_duplicate()
{
    std::map<std::string, std::string> tmp;

    for (int i = 0; i < (int)duplicate_in_location.size(); i++)
    {
        if (tmp.count(duplicate_in_location[i]) == 1)
            return 0;
        tmp[duplicate_in_location[i]] = "";
    }
    return 1;
}

int location::Index(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    location.index = hold[1];
    duplicate_in_location.push_back("index");

    return 1;
} 

int location::cgi_state(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.cgi = true;
    else if (hold[1] == "off")
        location.cgi = false;
    else
        return 0;
    duplicate_in_location.push_back("cgi");
    return 1;
}

int location::upload_state(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0;
    if (hold[1] == "on")
        location.upload_s = true;
    else if (hold[1] == "off")
        location.upload_s = false;
    else
        return 0;
    duplicate_in_location.push_back("u_state");
    return 1;
}

int location::uploadpath(location &location, std::vector<std::string> &hold)
{
    if (hold.size() != 2)
        return 0; 
    if(access(hold[1].c_str(), F_OK) == -1)
        return 0;
    location.upload_path = hold[1];
    if (hold[1][hold[1].length() - 1] != '/')
        return (0);
    duplicate_in_location.push_back("u_path");
    return 1;
}

location::~location()
{}