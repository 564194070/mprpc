#include<iostream>
#include"mprpcapplication.h"
#include"friend.pb.h"
#include"mprpcchannel.h"
#include"mprpccontroller.h"

int main(int argc,char** argv)
{
    //整个框架启动后 想使用mprpc框架享受rpc调用，一定需要先调用框架初始化函数
    //框架只初始化一次

    MprpcApplication::Init(argc,argv);

    //rpc提供服务者 __Stub消费者
    //演示调用远程方法发布的rpc方法Login
    //构造函数是一个rpcChannel
    XATU::FiendServiceRpc_Stub stub(new MprpcChannel());
    //RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    //rpc方法的请求参数
    XATU::GetFriendsListRequest request;
    request.set_userid(1000);
    //rpc方法的响应
    XATU::GetFriendsListResponse response;
  
    MprpcController controller;    
    //发起rpc方法的调用，同步的rpc调用过程 MprpcChannel::callmethod
    stub.GetFriendsList(&controller,&request,&response,nullptr);

    if(controller.Failed())
    {
        //rpc中出现错误了
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        //rpc中不出现错误
         //一次rpc调用完成，读rpc调用结果
        if(0 == response.result().errcode())
        {
            //调用成功
            std::cout << "rpc login response" << response.sucess() << std::endl;   
            int size = response.friends_size();
            for(int i = 0;i < size;++i)
            {
                std::cout << "index" <<(i+1) << "name" << response.friends(i) << std::endl;
            }
        }
        else
        {
            std::cout << "rpc login response error :" <<response.result().errmsg() <<std::endl;
        }

    }
   
    return 0;
}