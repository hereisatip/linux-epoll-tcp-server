#include "protocol.hpp"

#include<iostream>
#include<string>

bool Check(const string& message,const string& smessage,const string& rmessage)
{
    if(message!=rmessage)
    {
        cerr<<smessage<<" is failed!"<<endl;
        cerr<<"actual receive: "<<message<<endl;

        return false;
    }
    cout<<smessage<<" is pass!"<<endl;
    return true;

}

int main()
{
    bool all_pass=true;

    all_pass&=Check(make_response("PING"),"PING","PONG\n");

    all_pass&=Check(make_response("Hello"),"Hello","server received: Hello\n");

    all_pass&=Check(make_response("hello world"),"hello world","server received: hello world\n");
    
    

    return 0;
}