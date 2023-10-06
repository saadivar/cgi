#include "Request.hpp"

void Request::ft_split(std::string input, std::string delimiter, std::vector<std::string> &parts)
{
    // size_t startPos = 0;
    // size_t endPos;
 
    // while ((endPos = input.find(delimiter, startPos)) != std::string::npos)
    // {
    //     parts.push_back(input.substr(startPos, endPos - startPos));
    //     startPos = endPos + delimiter.length();
    // }
    // parts.push_back(input.substr(startPos));
    size_t startPos = 0;
    size_t endPos;
    std::string tmp;

    while ((endPos = input.find(delimiter, startPos)) != std::string::npos)
    {
        tmp = input.substr(startPos, endPos - startPos);
        if(!tmp.empty())
            parts.push_back(tmp);
        startPos = endPos + delimiter.length();
    }
    tmp = input.substr(startPos);
    if(!tmp.empty())
            parts.push_back(tmp);
}

void Request::split_rquest_v2(std::vector<std::string> &r, std::string &req, char c)
{
    std::string tmp;
    std::istringstream tokensOfreq(req);

    std::getline(tokensOfreq, tmp, c);
    r.push_back(tmp);
    std::getline(tokensOfreq, tmp, '\n');
    r.push_back(tmp);
}

int Request::find_key(std::string key, std::vector<std::pair<std::string, std::string> > &postR)
{
    std::vector<std::pair<std::string, std::string> >::iterator it = postR.begin();

    for (; it != postR.end(); it++)
    {
        if (it->first == key)
            return 1;
    }
    return 0;
}

std::string Request::valueOfkey(std::string key, std::vector<std::pair<std::string, std::string> > &postR)
{
    std::vector<std::pair<std::string, std::string> >::iterator it = postR.begin();

    for (; it != postR.end(); it++)
    {
        if (it->first == key)
            return it->second;
    }
    return "";
}