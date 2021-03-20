#include<iostream>
#include<string>
#include"test.pb.h"
using namespace XATU;

//框架属于平台层，业务属于应用层
int main()
{
    //封装了Login请求对象的数据
    LoginRequest req;
    req.set_name("haibao");

    //数据对象序列化 char*
    std::string send_str;
    if(req.SerializePartialToString(&send_str))
    {
        std::cout << send_str.c_str() << std::endl;
    }
    
    //序列化 把对象转化成字节流或者字符流 和json不一样，紧凑排列
    //同样的数据 占的空间少
    LoginResponse rep;
    rep.set_errcode(111);

    //从send_str 反序列化一个login请求对象。
    LoginRequest reqB;
    reqB.ParseFromString(send_str);

    if(req.SerializePartialToString(&send_str))
    {
        std::cout <<reqB.name() << std::endl;
    }

    //protobuf 内部存在变量是一个类
    LoginRequest rsp;
    ResultCode *rc = rsp.mutable_result

    return 0;
}