#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include<cstdint>
#include<string>

using int64=uint64_t;



class CalcRequest
{
    public:
    int64 store_number;
    int store_delay_ms;
};

using namespace std;

    //保存计算数字number和可选模拟耗时delay_ms
    int CalcRequest();
    //判断是否以CALC开头
    bool is_calc_command(const string& message);
    //解析CALC请求;成功返回true,失败返回false并填写错误响应
    bool parse_calc_request(const string& message,int64& number,int& delay_ms);
    //处理PING,空消息和普通文本
    string make_response(const string& message);
    //生成1+..+number的结果响应
    string make_calc_response(int64& number);




#endif