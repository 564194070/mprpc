#include<iostream>
#include"mprpcchannel.h"
#include"mprpcapplication.h"
#include"user.pb.h"

using namespace std;

int main(int argc,char** argv)
{
    //整个框架启动后 想使用mprpc框架享受rpc调用，一定需要先调用框架初始化函数
    //框架只初始化一次

    MprpcApplication::Init(argc,argv);

    //rpc提供服务者 __Stub消费者
    //演示调用远程方法发布的rpc方法Login
    //构造函数是一个rpcChannel
    XATU::UserServiceRpc_Stub stub(new MprpcChannel());
    //RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    //rpc方法的请求参数
    XATU::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    //rpc方法的响应
    XATU::LoginResponse response;
    //发起rpc方法的调用，同步的rpc调用过程 MprpcChannel::callmethod
    stub.Login(nullptr,&request,&response,nullptr);

    //一次rpc调用完成，读rpc调用结果
    if(0 == response.result().errcode())
    {
        //调用成功
        std::cout << "rpc login response" << response.sucess() << std::endl;   
    }
    else
    {
        std::cout << "rpc login response error :" <<response.result().errmsg() <<std::endl;
    }

    //演示调用远程发布大的rpc方法
    XATU::RegisterRequest req;
    req.set_id(2000);
    req.set_name("mprpc");
    req.set_pwd("999999");
    XATU::RegisterResponse rsp;
    
    //以东部的方式发起rpc请求，等待返回结果
    stub.Register(nullptr,&req,&rsp,nullptr);

    //一次rpc调用完成，读rpc调用结果
    if(0 == rsp.result().errcode())
    {
        //调用成功
        std::cout << "rpc register response" << response.sucess() << std::endl;   
    }
    else
    {
        std::cout << "rpc register response error :" <<response.result().errmsg() <<std::endl;
    }


    return 0;
}