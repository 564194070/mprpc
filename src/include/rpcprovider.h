#pragma once
#include"google/protobuf/service.h"
#include<memory>
#include<string>
#include<functional>
#include<google/protobuf/descriptor.h>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
#include<unordered_map>
//框架提供的专门用发来布rpc网络对象类
class RpcProvider
{
    public:
        //框架代码上 用基类指针而不是派生类指针，因为基类指针可以指向所有派生类对象
        //框架开发不能依赖与具体的某些业务
        //Service抽象类指针可以继承任何从他继承来的对象

        //框架提供给外部使用，可以发布rpc方法的函数接口,
        //protobuf生成的rpc方法都是从Service继承而来的，指向所有从这个类继承而来的对象
        void NotifyService(google::protobuf::Service * service);

        //启动rpc服务节点，开始提供rpc远程网络调用服务
        void Run();
    private:
        //组合了一个TcpServer
        //std::unique_ptr<muduo::net::TcpServer> m_tcpserver_ptr;
        //组合了Eventloop
        muduo::net::EventLoop m_eventLoop;

        //新的socket链接回调
        void OnConnection(const muduo::net::TcpConnectionPtr&);
        //已建立连接用户的读写事件回调
        void OnMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);    

        //service服务类型信息
        struct ServiceInfo
        {
            //保存服务对象
            google::protobuf::Service* m_service;
            //保存服务方法
            std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_methodMap;
        };

        std::unordered_map<std::string,ServiceInfo> m_serviceMap;
        //Closure回调操作，用于序列化rpc的响应和网络发送
        void SendRpcResponse(const muduo::net::TcpConnectionPtr&,google::protobuf::Message*); 
         

};      