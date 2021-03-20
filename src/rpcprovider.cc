#include"./rpcprovider.h"
#include"mprpcapplication.h"
#include<google/protobuf/message.h>
#include"rpcheader.pb.h"


//这里是框架提供给外部使用的

    //框架提供给外部使用的，可以发布rpc方法的接口
    //把服务拆分成最小，根据服务对硬件资源的需求，进行灵活部署
    //提供rpc方法你就成了rpc服务器 请求rpc方法你就成了rpc客户端

    //提供了良好的序列化 反序列化 和对rpc业务的支持
    //Service 描述对象 Method描述方法

void RpcProvider::NotifyService(google::protobuf::Service * service)
{
    ServiceInfo service_info;


    //获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    //获取服务的名字
    std::string service_name = pserviceDesc->name();
    //获取服务对象service方法的数量
    int methodCnt = pserviceDesc->method_count();
    std::cout << "service_name" <<service_name<< std::endl;


    for(int i = 0; i < methodCnt; ++i)
    {
        //获取了服务对象指定下标的服务方法的描述
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name,pmethodDesc});
        std::cout << "method_name" <<method_name<< std::endl;    
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name,service_info});
}

//启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    muduo::net::InetAddress address(ip,port); 
    //创建Tcp对象库 将方法封装在Run方法中
    muduo::net::TcpServer server(&m_eventLoop,address,"RpcProvider");

    //epoll+多线程
    //绑定连接回调和消息读写回调方法 分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection,this,std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    //设置muduo库的线程数量
    server.setThreadNum(4);

    std::cout << "Tcpserver start ip" << ip << "port " << port <<std::endl;
    //启动网络服务
    server.start();
    //以阻塞的方式等待远程连接
    m_eventLoop.loop();

}
//新的socket链接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr&conn)
{
    //三次握手直接回调
    //rpc请求和http一样，是一个短链接的请求
    if(!conn->connected())
    {
        //和rpc client断开了连接
        //关闭文件描述符
        conn->shutdown();
    }
}

/*
    在框架内部，RpcProvider和RpcConsumer协商好一个rpc服务调用请求，OnMessage就会响应
    service_name method_name args
    定义proto的message类型，进行数据头的序列化和反序列化

    header_size head_str args_str
    service_name method_name args_size
    不记录长度会出现粘包问题
    服务名字 方法名字
*/

//已建立连接用户的读写事件回调
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr&conn
                            ,muduo::net::Buffer* buffer
                            ,muduo::Timestamp time)
{
    //收发数据直接回调
    //已建立连接用户的读写事件回调，如果远程有一个rpc服务调用请求，那么onmessage方法就会响应

    //网络上接受的远程rpc请求字符流 rpc方法名字/参数
    std::string recv_buf = buffer->retrieveAllAsString();

    //string insert/copy 按照内存大小处理数据就很有用
    //从字符流中读取整数的前四个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size,4,0);

    //根据head_size读取数据头的原始字符流 反序列化数据，得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4,header_size);
    mprpc::RpcHeader rpcHeader;

    //可以取出来的数据
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if(rpcHeader.ParseFromString(rpc_header_str))
    {
        //数据头 反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        //数据头 反序列化失败
        std::cout << "rpc_header_str" << rpc_header_str << "parse error" << std::endl;
        return ;
    }
    //获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4+header_size,args_size);


    //打印调试信息
    std::cout << "head_size" << header_size << std::endl;
    std::cout <<"rpc_header_str" << rpc_header_str << std::endl;
    std::cout << "service_name" << service_name << std::endl;
    std::cout << "method_name" << method_name << std::endl;
    std::cout << "args_nums" << args_str << std::endl;

    //获取service对象和method对象
    auto it = m_serviceMap.find(service_name);

    if(it == m_serviceMap.end())
    {
        //在rpc节点上请求了一个不存在的业务
        std::cout << service_name << " is not exist" << std::endl;
        return ;
    }
    
    auto mit = it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end())
    {
        std::cout << service_name << ":" << method_name << "is not exist" << std::endl;
        return ;
    }

    //获取service对象 获取method对象
    google::protobuf::Service *service = it->second.m_service;  //发布的service服务
    const google::protobuf::MethodDescriptor* method = mit->second; //对象的方法

    //生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    if(request->ParseFromString(args_str))
    {

    }
    else
    {
        std::cout << "request parse error,content" << args_str << std::endl;
        return ;
    }
    //给下面的method方法的调用，绑定一个Clousure
    //google::protobuf::NewCallback(); 
    google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>(this,&RpcProvider::SendRpcResponse,conn,response);

    //在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    //new UserService().Login(controller,request,response,done)
    service->CallMethod(method,nullptr,request,response,done);


}  

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr&conn,google::protobuf::Message*response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        //序列化成功后 通过网络把rpc方法执行的结果发送给rpc的调用方
        conn->send(response_str);

    }
    else
    {
        std::cout << "serialize response_str error,content" << std::endl;
        
    }
    //模拟http的短链接服务 由rpcprovider主动断开连接
    conn->shutdown();

}

//服务提供方 会首先通过Notify 后来启动成一个epoll+服务器

/*
    1. 服务提供方通过rpc provider上注册服务对象和服务方法
    2. 抽象层的Method和service把服务对象和他调用的服务方法记录在Map表中
    3. 启动之后 启动了epoll+多线程服务器
    4. 新连接onconnection方法
    5. OnMessage等待远程rpc调用请求 按照协商好的数据格式解析
    6.通过抽相层动态生成请求和响应，把参数数据反序列化到请求中
    7.业务填写response 生成回调
    8.框架调用业务方法 

*/