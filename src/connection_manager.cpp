#include"connection_manager.hpp"

#include<vector>

bool connection_manager::add(int fd)
{
    if(fd<0)
    {
        return false;
    }
    if(client_session.find(fd)!=client_session.end())
    {
        return false;
    }
    ClientSession cs;

    cs.fd=fd;

    cs.connection_id=connection_number;
    connection_number++;

    if(connection_number==0)
    {
        connection_number=1;
    }

    client_session[cs.fd]=cs;
    return true;
    
}

bool connection_manager::contains(size_t fd)
{
    return client_session.find(fd)!=client_session.end();
}

ClientSession* connection_manager::find(size_t fd)
{
    auto cs=client_session.find(fd);
    if(cs==client_session.end())
    {
        return nullptr;
    }
    return &cs->second;
}

bool connection_manager::remove(size_t fd)
{
    if(contains(fd))
    {
        client_session.erase(fd);
        return true;
    }
    return false;
}

size_t connection_manager::size()
{
    return client_session.size();
}

vector<int> connection_manager::file_descriptors()
{
    vector<int> result;

    result.reserve(client_session.size());

    for(auto &[fd,session]:client_session)
    {
        result.push_back(fd);
    }
    return result;
}

void connection_manager::clear()
{
    client_session.clear();
}
