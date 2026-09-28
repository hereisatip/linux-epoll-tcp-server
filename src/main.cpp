#include"server.hpp"
#include<csignal>



int main()
{
    

    Server server(9000);
    server.run(0);
    
    return 0;
}