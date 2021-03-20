#include<iostream>
#include<string>
#include"mprpcapplication.h"
#include"rpcprovider.h"
#include"friend.pb.h"
#include"./logger.h"


class FriendService:public XATU::FiendServiceRpc
{
public:
    std::vector<std::string> GetFriendsList(uint32_t userid)
    {
        std::cout << "do getFriendsList" << std::endl;
        std::vector<std::string> vec;
        vec.push_back("aaa");
        vec.push_back("bbb");
        vec.push_back("ccc");

        return vec;
    }

    //重写基类方法
    void GetFriendsList(::google::protobuf::RpcController* controller,
                       const ::XATU::GetFriendsListRequest* request,
                       ::XATU::GetFriendsListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        //1.取出数据
        uint32_t userid = request->userid();
        std::vector<std::string> friendsList = GetFriendsList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for(std::string& name:friendsList)
        {
            std::string *p = response->add_friends();
            *p = name;
        }
        done->Run();
    }
};


int main(int argc,char** argv)
{
    LOG_INFO()
    FriendService us;
    //调用当前业务 没有问题 其他进程怎么调用？
    //设计成远程rpc调用 分布式调用
    //us.Login("xxx","123");


    //调用框架初始化操作 服务器和zookerper的端口号
    //provider -i config.confo
    MprpcApplication::Init(argc,argv);

    //服务对象 很多人请求 要高并发 用muduo库
    RpcProvider provider;
    //发布服务 把Servervice对象发送到rpc节点上
    //是一个网络服务对象 发布rpc方法
    provider.NotifyService(new FriendService());
    //启动一个服务发布结点
    //进程进入阻塞状态 等待远程rpc请求
    provider.Run();

}