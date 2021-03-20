#include<iostream>
#include<string>
#include"../user.pb.h"
#include"mprpcapplication.h"
#include"rpcprovider.h"

//1.proto 2.生成代码 3.注意名字空间 4.Rpc发布者用 Rpc_Stub服务者用
//本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
//使用在rpc服务发布端的 rpc服务提供者
class UserService:public XATU::UserServiceRpc
{
    public:
        bool Login(std::string name,std::string pwd)
        {
            std::cout << "doing Login service: Login" << std::endl;
            std::cout << "name" << name << std::endl;
            std::cout << "pwd"<< pwd <<std::endl;
            return true;
        }


        //重写基类rpc的虚函数 方法是框架调用的

        //客户发来的rpc请求先被框架所接受，匹配方法 
        /*
            caller调用者==========》 发起请求Login（LoginRequest） 打包发送给提供方========》callee
            callee ===============》 Login（LoginRequest）交到你重写的方法上来 ============》callee
        */
        void Login(::google::protobuf::RpcController* controller,
                       const ::XATU::LoginRequest* request,
                       ::XATU::LoginResponse* response,
                       ::google::protobuf::Closure* done)
        {
            //框架给业务上报了请求参数 Request，框架解析放序列化
            //业务获取相应的数据做本地业务
            std::string name = request->name();
            std::string pwd = request->pwd();

            //有效性检查
            //本地业务
            bool login_result = Login(name,pwd);

            //把响应给框架返回 包括 错误码 错误消息 返回值
            XATU::ResultCode *code = response->mutable_result();
            code->set_errcode(0);
            code->set_errmsg("no error");
            response->set_sucess(login_result);

            //执行回调操作
            //Closure 抽象类 定义一个继承的方法处理Run
            //响应对象数据的序列化和网络发送
            done->Run();

            /*
                1.从LoginRequest获取参数的值
                2.执行本地login，并获取返回值
                3.用上面返回值填写LoginResponse
                4.一个回调 把LoginResponse发送给rpc client
            */
        }
    private:

};

int main(int argc,char** argv)
{
    UserService us;
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
    provider.NotifyService(new UserService());
    //启动一个服务发布结点
    //进程进入阻塞状态 等待远程rpc请求
    provider.Run();

}