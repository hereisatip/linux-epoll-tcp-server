#include "protocol.hpp"

#include<sstream>
#include<vector>
#include<cctype>

int64 max_number=10000;
int max_delay_ms=10000;

bool is_unsigned_number(const string& text)
{
    if(text.empty())
    {
        return false;
    }

    for(auto letter:text)
    {
        if(!isdigit(letter))
        {
            return false;
        }
    }
    return true;
}

bool parse_unsigned_number(const string& text,int64& number)
{
    if(!is_unsigned_number(text))
    {
        return false;
    }

    try
    {
        size_t parsed_length=0;

        number=stoull(text,&parsed_length);

        if(parsed_length!=text.size())
        {
            return false;
        }
    }
    catch(...)
    {
        //字符串可能太长或者转换失败
        return false;
    }
    
    return true;

}

vector<string> split_fields(const string& message)
{
    istringstream iss(message);
    vector<string> fields;
    string field;

    while(iss>>field)
    {
        fields.push_back(field);
    }

    return fields;
} 


bool is_calc_command(const string& message)
{
    vector<string> fields=split_fields(message);

    return !fields.empty()&&fields.front()=="CALC";
}

bool parse_calc_request(const string& message,int64& number,int& delay_ms)
{
    vector<string> fields=split_fields(message);

    //CALC N
    //CALC N delay_ms

    if(fields.size()<2||fields.size()>3)
    {
        return false;
    }
    
    if(number>max_number)
    {
        return false;
    }

    if(delay_ms>max_delay_ms)
    {
        return false;
    }

    if(!parse_unsigned_number(fields[1],number))
    {
        return false;
    }

    if(fields.size()==3 && !parse_unsigned_number(fields[2],(int64&)delay_ms))
    {
        return false;
    }

    return true;

}   

string make_calc_response(int64& number)
{
    int64 result=number*(number+1)/2;

    return "RESULT: "+to_string(result)+"\n";
}

string make_response(const string& message)
{
    if(message=="PING")
    {
        return "PONG\n";
    }

    if(is_calc_command(message))
    {
        vector<string> fields=split_fields(message);
        
        if(fields.size()<2)
        {
            return "ERROR invalid number\n";
        }
        else if(fields.size()>3)
        {
            return "ERROR invalid argument\n";
        }

        int64 number=0;

        if(!parse_unsigned_number(fields[1],number))
        {
            return "ERROR invalid number\n";
        }

        if(number>max_number)
        {
            return "ERROR invalid number\n";
        }

        return make_calc_response(number);
    }

    return "server received: "+message+"\n";
}
