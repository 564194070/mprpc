#include"logger.h"
#include<time.h>
#include<iostream>


//获取日志的单例模式
Logger& Logger::getInstance()
{
    static Logger logger;
    return logger;
}

//设置日志级别
void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = level;
}


//启动专门的写日志线程
Logger::Logger()
{
    //启动专门的写日志线程
    std::thread writeLogTask([&]()
    {
        for(;;)
        {
            //1.获取当天日期，写入相应的日志文件当中 a+
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);
            char filename[128];
            sprintf(filename,"%d-%d-%d-log.txt",nowtm->tm_year+1900,nowtm->tm_mon+1,nowtm->tm_mday);

            FILE *pf = fopen(filename,"a+");
            if(pf == NULL)
            {
                std::cout << "Logger file :" <<filename << "open error" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lckQue.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf,"%d-%d-%d=>",nowtm->tm_hour,nowtm->tm_min,nowtm->tm_sec);
            msg.insert(0,time_buf);
            
            fputs(msg.c_str(),pf);
            fclose(pf);
        }
    });

    //设置分离线程 守护线程
    writeLogTask.detach();

}

//写日志 吧日志写入lockqueue缓冲区当中
void Logger::Log(std::string msg)
{
    m_lckQue.Push(msg);
}
