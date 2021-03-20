#pragma once

#include"mprpcconfig.h"

//负责框架的一些初始化操作
//mprpc框架的初始化类
class MprpcApplication
{
    public:
        static void Init(int argc,char **argv);
        static MprpcApplication& GetInstance();
        static MprpcConfig& GetConfig();

    private:
        MprpcApplication(){};
        MprpcApplication(const MprpcApplication&) = delete;
        MprpcApplication(MprpcApplication&&) = delete;

        static MprpcConfig m_config;
};