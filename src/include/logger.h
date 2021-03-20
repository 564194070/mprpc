#pragma once

#include"lockqueue.h"

enum LogLevel
{
    INFO,   //普通信息
    ERROR,  //错误信息
};
//框架提供的日志系统
class Logger
{
public:
    //获取日志的单例模式
    static Logger& getInstance();
    //设置日志级别
    void SetLogLevel(LogLevel level);
    //写日志
    void Log(std::string msg);
private:
    //记录日志级别
    int m_loglevel;
    //日志缓冲队列
    LockQueue<std::string> m_lckQue;

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger &&) = delete;
};

// 定义宏 可变参 日志写入方法
#define LOG_INFO(logmsgformat,...)\
    do \
    {  \
        Logger &logger = Logger::getInstance();\
        logger.SetLogLevel(INFO);   \
        char c[1024] = {0};         \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0);

#define LOG_ERR(logmsgformat,...)\
    do \
    {  \
        Logger &logger = Logger::getInstance();\
        logger.SetLogLevel(ERROR);   \
        char c[1024] = {0};         \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0);
